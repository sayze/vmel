rm -rf build

echo "Creating root build directory..."

mkdir -p build

cd build

cmake "-DCMAKE_BUILD_TYPE=Release" ..

make
