#Make required dirs.

echo "Building shared libraries (modules)"

cd modules 

bash install.sh

echo "Creating root build directory..."

cd ..

mkdir -p build

cd build

cmake "-DCMAKE_BUILD_TYPE=Release" ..

make
