// Copyright 2020 PostgreSQL Global Development Group

#[macro_use]
extern crate clap;
extern crate chrono;
extern crate rand;
extern crate rand_pcg;
extern crate regex;

use std::boxed::Box;
use std::fs;
use std::io::{self, BufRead, BufReader, Write};
use std::process;
use std::vec::Vec;

use chrono::{NaiveDate, NaiveDateTime};

use rand::{Rng, SeedableRng};

use regex::Regex;

const ALPHA: &[u8] = b"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

const TYPE_DATE: char = 'd';
const TYPE_EXPONENTIAL: char = 'e';
const TYPE_GAUSSIAN: char = 'g';
const TYPE_INTEGER: char = 'i';
const TYPE_LIST: char = 'l';
const TYPE_POISSON: char = 'p';
const TYPE_SEQUENCE: char = 's';
const TYPE_TEXT: char = 't';

struct TableDefinition {
    rows: u64,
    columns: Vec<Box<dyn Column>>,
}

// Design each column type such that it will call the global random number
// generator a constant number of times, and return that number of times in
// count().  This allows us to know how much to advance the random number
// generator when only generating a portion of the data.  If the number of
// random numbers needed is not constant, create a second random number
// generator that is seeded by the global random number generator so that the
// data generated is consistent.
trait Column {
    fn column_type(&self) -> char;
    fn count(&self) -> u8;
    fn to_int(&self, rng: &mut rand_pcg::Pcg64) -> Option<i128>;
    fn to_string(&self, rng: &mut rand_pcg::Pcg64) -> Option<String>;
}

struct Date {
    start: NaiveDateTime,
    end: NaiveDateTime,
}

impl Column for Date {
    fn column_type(&self) -> char {
        return TYPE_DATE;
    }

    fn count(&self) -> u8 {
        return 1;
    }

    fn to_int(&self, _rng: &mut rand_pcg::Pcg64) -> Option<i128> {
        return None;
    }

    fn to_string(&self, rng: &mut rand_pcg::Pcg64) -> Option<String> {
        let sometime: i64 = self.start.timestamp()
            + ((self.end.timestamp() - self.start.timestamp() + 1) as f64 * rng.gen::<f64>())
                as i64;
        let ndt: NaiveDateTime = NaiveDateTime::from_timestamp(sometime, 0);
        return Some(ndt.format("%Y-%m-%d").to_string());
    }
}

struct Exponential {
    cut: f64,
    min: i128,
    max: i128,
    probability: i128,
}

impl Column for Exponential {
    fn column_type(&self) -> char {
        return TYPE_EXPONENTIAL;
    }

    fn count(&self) -> u8 {
        return 1;
    }

    fn to_int(&self, rng: &mut rand_pcg::Pcg64) -> Option<i128> {
        let uniform: f64 = 1.0 - rng.gen::<f64>();
        let rand: f64 = -((self.cut + (1.0 - self.cut) * uniform).ln()) / self.probability as f64;
        return Some(self.min + ((self.max - self.min + 1) as f64 * rand) as i128);
    }

    fn to_string(&self, _rng: &mut rand_pcg::Pcg64) -> Option<String> {
        return None;
    }
}

struct Gaussian {
    min: i128,
    max: i128,
    probability: f64,
}

impl Column for Gaussian {
    fn column_type(&self) -> char {
        return TYPE_GAUSSIAN;
    }

    fn count(&self) -> u8 {
        return 1;
    }

    fn to_int(&self, rng: &mut rand_pcg::Pcg64) -> Option<i128> {
        let mut rng2 = rand_pcg::Pcg64::seed_from_u64(rng.gen::<u64>());
        loop {
            let rand1: f64 = rng2.gen::<f64>();
            let rand2: f64 = rng2.gen::<f64>();
            let var_sqrt: f64 = (-2.0 * rand1.ln()).sqrt();
            let stdev: f64 = var_sqrt * (2.0 * std::f64::consts::PI * rand2).sin();

            if stdev >= -self.probability && stdev < self.probability {
                let rand: f64 = (stdev + self.probability) / (self.probability * 2.0);
                return Some(self.min + ((self.max - self.min + 1) as f64 * rand) as i128);
            }
        }
    }

    fn to_string(&self, _rng: &mut rand_pcg::Pcg64) -> Option<String> {
        return None;
    }
}

struct Integer {
    min: i128,
    max: i128,
}

impl Column for Integer {
    fn column_type(&self) -> char {
        return TYPE_INTEGER;
    }

    fn count(&self) -> u8 {
        return 1;
    }

    fn to_int(&self, rng: &mut rand_pcg::Pcg64) -> Option<i128> {
        return Some(self.min + ((self.max - self.min + 1) as f64 * rng.gen::<f64>()) as i128);
    }

    fn to_string(&self, _rng: &mut rand_pcg::Pcg64) -> Option<String> {
        return None;
    }
}

struct List {
    items: Vec<String>,
}

impl Column for List {
    fn column_type(&self) -> char {
        return TYPE_LIST;
    }

    fn count(&self) -> u8 {
        return 1;
    }

    fn to_int(&self, _rng: &mut rand_pcg::Pcg64) -> Option<i128> {
        return None;
    }

    fn to_string(&self, rng: &mut rand_pcg::Pcg64) -> Option<String> {
        let i: usize = (self.items.len() as f64 * rng.gen::<f64>()) as usize;
        return Some(self.items[i].to_string());
    }
}

struct Poisson {
    center: i128,
}

impl Column for Poisson {
    fn column_type(&self) -> char {
        return TYPE_POISSON;
    }

    fn count(&self) -> u8 {
        return 1;
    }

    fn to_int(&self, rng: &mut rand_pcg::Pcg64) -> Option<i128> {
        let uniform: f64 = 1.0 - rng.gen::<f64>();
        return Some((-uniform.ln() * self.center as f64 + 0.5) as i128);
    }

    fn to_string(&self, _rng: &mut rand_pcg::Pcg64) -> Option<String> {
        return None;
    }
}

struct Sequence {
    start: i128,
}

impl Column for Sequence {
    fn column_type(&self) -> char {
        return TYPE_SEQUENCE;
    }

    fn count(&self) -> u8 {
        return 0;
    }

    fn to_int(&self, _rng: &mut rand_pcg::Pcg64) -> Option<i128> {
        return Some(self.start);
    }

    fn to_string(&self, _rng: &mut rand_pcg::Pcg64) -> Option<String> {
        return None;
    }
}

struct Text {
    min: i128,
    max: i128,
}

impl Column for Text {
    fn column_type(&self) -> char {
        return TYPE_TEXT;
    }

    fn count(&self) -> u8 {
        return 1;
    }

    fn to_int(&self, _rng: &mut rand_pcg::Pcg64) -> Option<i128> {
        return None;
    }

    fn to_string(&self, rng: &mut rand_pcg::Pcg64) -> Option<String> {
        let mut rng2 = rand_pcg::Pcg64::seed_from_u64(rng.gen::<u64>());
        let length: i128 =
            self.min + ((self.max - self.min + 1) as f64 * rng2.gen::<f64>()) as i128;
        let alpha: String = (0..length)
            .map(|_| {
                let idx = rng2.gen_range(0, ALPHA.len());
                ALPHA[idx] as char
            })
            .collect();
        return Some(alpha);
    }
}

fn generate_data(
    table: TableDefinition,
    delimiter: String,
    chunks: u64,
    chunk: u64,
    seed: u64,
) -> std::io::Result<()> {
    let chunk_size = table.rows / chunks;
    let chunk_start = (chunk - 1) * chunk_size;
    let chunk_end = if chunk == chunks {
        table.rows
    } else {
        chunk_start + chunk_size
    };

    if chunk_size > table.rows || chunk_end > table.rows {
        eprintln!("ERROR: chunking parameters do not make sense with specified rows");
        eprintln!("       rows: {}", table.rows);
        eprintln!("       chunk: {}", chunk);
        eprintln!("       chunks: {}", chunks);
        process::exit(1);
    }

    let mut rng = rand_pcg::Pcg64::seed_from_u64(seed);

    let mut row = 0;

    // Simply advanced the random number generator if generating just a portion
    // of the data.
    let mut count = 0;
    for c in 0..table.columns.len() {
        count += table.columns[c].count();
    }
    loop {
        if row == chunk_start {
            break;
        }

        for _i in 0..count {
            rng.gen::<f64>();
        }

        row += 1;
    }

    let stdout = io::stdout();
    let mut handle = io::BufWriter::new(stdout);

    // Output generated data.
    loop {
        for c in 0..table.columns.len() {
            if table.columns[c].column_type() == TYPE_DATE {
                write!(handle, "{}", table.columns[c].to_string(&mut rng).unwrap())?;
            } else if table.columns[c].column_type() == TYPE_EXPONENTIAL {
                write!(handle, "{}", table.columns[c].to_int(&mut rng).unwrap())?;
            } else if table.columns[c].column_type() == TYPE_GAUSSIAN {
                write!(handle, "{}", table.columns[c].to_int(&mut rng).unwrap())?;
            } else if table.columns[c].column_type() == TYPE_INTEGER {
                write!(handle, "{}", table.columns[c].to_int(&mut rng).unwrap())?;
            } else if table.columns[c].column_type() == TYPE_LIST {
                write!(handle, "{}", table.columns[c].to_string(&mut rng).unwrap())?;
            } else if table.columns[c].column_type() == TYPE_POISSON {
                write!(handle, "{}", table.columns[c].to_int(&mut rng).unwrap())?;
            } else if table.columns[c].column_type() == TYPE_SEQUENCE {
                write!(
                    handle,
                    "{}",
                    row as i128 + table.columns[c].to_int(&mut rng).unwrap()
                )?;
            } else if table.columns[c].column_type() == TYPE_TEXT {
                write!(handle, "{}", table.columns[c].to_string(&mut rng).unwrap())?;
            } else {
                eprintln!(
                    "\nERROR: unhandled column type: {}",
                    table.columns[c].column_type()
                );
                process::exit(1);
            }

            if (c + 1) < table.columns.len() {
                write!(handle, "{}", delimiter)?;
            } else {
                // End of the row.
                writeln!(handle)?;
            }
        }

        row += 1;
        if row == chunk_end {
            break;
        }
    }
    Ok(())
}

fn main() -> std::io::Result<()> {
    let matches = clap_app!(touchstone =>
        (@arg CHUNKS: -c +takes_value default_value("1") "number of data file chunks to generate")
        (@arg CHUNK: -C +required +takes_value "specify which chunk to generate")
        (@arg DELIMITER: -d +takes_value default_value("	") "column delimiter")
        (@arg FILENAME: -f +required +takes_value "data definition file")
        (@arg SEED: -s +takes_value "set seed [default: random]")
    )
    .get_matches();

    let seed: u64 = if matches.value_of("SEED").is_none() {
        rand::random()
    } else {
        matches.value_of("SEED").unwrap().parse::<u64>().unwrap()
    };

    let filename = matches.value_of("FILENAME").unwrap().to_string();
    let table = read_data_definition_file(filename);

    let delimiter = matches.value_of("DELIMITER").unwrap().to_string();
    let chunks = matches.value_of("CHUNKS").unwrap().parse::<u8>().unwrap();
    let chunk = matches.value_of("CHUNK").unwrap().parse::<u8>().unwrap();
    generate_data(table, delimiter, chunks as u64, chunk as u64, seed)?;
    Ok(())
}

fn read_data_definition_file(filename: String) -> TableDefinition {
    let file = fs::File::open(filename).unwrap();
    let mut reader = BufReader::new(file);
    let mut buf = String::new();

    let mut columns: Vec<Box<dyn Column>> = Vec::new();

    // The first line of the data definition file is the number of rows to
    // generate.
    reader
        .read_line(&mut buf)
        .expect("error reading from table definition file");
    let rows: u64 = buf.trim().parse::<u64>().unwrap();
    buf.clear();

    // The remaining rows in the data definition file correspond to each column
    // definition.
    loop {
        let length = reader
            .read_line(&mut buf)
            .expect("error reading from table definition file");
        if length == 0 {
            break;
        }

        let definition: Vec<char> = buf.trim().chars().collect();
        if definition[0] == TYPE_DATE {
            let re = Regex::new(r"d(\d+)-(\d+)-(\d+),(\d+)-(\d+)-(\d+)").unwrap();
            let cap = re.captures(&buf).unwrap();
            let c = Date {
                start: NaiveDate::from_ymd(
                    cap[1].parse::<i32>().unwrap(),
                    cap[2].parse::<u32>().unwrap(),
                    cap[3].parse::<u32>().unwrap(),
                )
                .and_hms(0, 0, 0),
                end: NaiveDate::from_ymd(
                    cap[4].parse::<i32>().unwrap(),
                    cap[5].parse::<u32>().unwrap(),
                    cap[6].parse::<u32>().unwrap(),
                )
                .and_hms(0, 0, 0),
            };
            columns.push(Box::new(c));
        } else if definition[0] == TYPE_EXPONENTIAL {
            let re = Regex::new(r"e(\d+),(\d+),(\d+)").unwrap();
            let cap = re.captures(&buf).unwrap();
            let c = Exponential {
                cut: (-cap[3].parse::<f64>().unwrap()).exp(),
                min: cap[1].parse::<i128>().unwrap(),
                max: cap[2].parse::<i128>().unwrap(),
                probability: cap[3].parse::<i128>().unwrap(),
            };
            columns.push(Box::new(c));
        } else if definition[0] == TYPE_GAUSSIAN {
            let re = Regex::new(r"g(\d+),(\d+),(\S+)").unwrap();
            let cap = re.captures(&buf).unwrap();
            let c = Gaussian {
                min: cap[1].parse::<i128>().unwrap(),
                max: cap[2].parse::<i128>().unwrap(),
                probability: cap[3].parse::<f64>().unwrap(),
            };
            columns.push(Box::new(c));
        } else if definition[0] == TYPE_INTEGER {
            let re = Regex::new(r"i(\d+),(\d+)").unwrap();
            let cap = re.captures(&buf).unwrap();
            let c = Integer {
                min: cap[1].parse::<i128>().unwrap(),
                max: cap[2].parse::<i128>().unwrap(),
            };
            columns.push(Box::new(c));
        } else if definition[0] == TYPE_LIST {
            let re = Regex::new(r".(\S+)").unwrap();
            let cap = re.captures(&buf).unwrap();
            let listfile = fs::File::open(cap[1].to_string()).unwrap();
            let listreader = BufReader::new(listfile);
            let mut items: Vec<String> = Vec::new();
            for line in listreader.lines() {
                items.push(line.unwrap());
            }
            let c = List { items: items };
            columns.push(Box::new(c));
        } else if definition[0] == TYPE_POISSON {
            let re = Regex::new(r"p(\d+)").unwrap();
            let cap = re.captures(&buf).unwrap();
            let c = Poisson {
                center: cap[1].parse::<i128>().unwrap(),
            };
            columns.push(Box::new(c));
        } else if definition[0] == TYPE_SEQUENCE {
            let re = Regex::new(r"s(\d+)").unwrap();
            let cap = re.captures(&buf).unwrap();
            let c = Sequence {
                start: cap[1].parse::<i128>().unwrap(),
            };
            columns.push(Box::new(c));
        } else if definition[0] == TYPE_TEXT {
            let re = Regex::new(r"t(\d+),(\d+)").unwrap();
            let cap = re.captures(&buf).unwrap();
            let c = Text {
                min: cap[1].parse::<i128>().unwrap(),
                max: cap[2].parse::<i128>().unwrap(),
            };
            columns.push(Box::new(c));
        } else {
            eprintln!("ERROR: unrecognized column definition: {}", buf.trim());
            process::exit(1);
        }
        buf.clear();
    }

    let table = TableDefinition {
        rows: rows,
        columns: columns,
    };

    return table;
}
