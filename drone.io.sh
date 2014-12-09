mkdir -p build/release
pushd build/release

cmake -DCMAKE_BUILD_TYPE=Release ../../
make -j 6
