apt-get install libicu-dev
apt-get install libboost-all-dev

mkdir -p build/release
pushd build/release

cmake -DCMAKE_BUILD_TYPE=Release ../../
make -j 6
