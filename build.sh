export CXX=clang++-11
export CC=clang-11

cd build/
#conan install .. --build missing --profile clang
cmake .. -DCMAKE_C_COMPILER=$CC -DCMAKE_CXX_COMPILER=$CXX
cmake --build . --parallel
cd ..
