default:
	@echo "targets: clean, debug, package, release, test"

clean:
	-rm -rf build

debug:
	cmake -H. -Bbuild/debug -DCMAKE_BUILD_TYPE=Debug -DENABLE_CASSERT=1
	cd build/debug && make

package:
	git checkout-index --prefix=build/source/ -a
	cmake -Hbuild/source -Bbuild/source
	cd build/source && make package_source

release:
	cmake -H. -Bbuild/release
	cd build/release && make

test: debug
	cd build/debug && make test
