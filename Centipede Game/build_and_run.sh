#!/bin/bash

echo Removing existing build directory
rm -rf ./build

echo Creating new build directory
mkdir build
cd build

echo Building App
cmake ..
cmake --build .

echo Running App
cd ./output/bin
./Centipede