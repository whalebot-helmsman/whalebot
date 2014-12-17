sudo apt-get install libleveldb-dev
sudo apt-get install libsnappy-dev
sudo apt-get install libicu-dev
sudo apt-get install libboost-all-dev

mkdir -p build/release
pushd build/release

cmake -DCMAKE_BUILD_TYPE=Release ../../
make -j 6
