mkdir -p Build/Release
cd Build/Debug
cmake -DCMAKE_BUILD_TYPE=Debug ../.. && cmake --build .
