#!/bin/sh
mkdir -p build
cd build
cmake ..
make
./src/cli/grdb