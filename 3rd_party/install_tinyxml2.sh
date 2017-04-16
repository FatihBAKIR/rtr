#!/bin/bash

git clone https://github.com/leethomason/tinyxml2 --depth=1
cd tinyxml2
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
make install
