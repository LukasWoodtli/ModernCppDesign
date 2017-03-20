set -u
set -e

mkdir build
cd build
cmake -G 'Unix Makefiles' ..
make && make test
