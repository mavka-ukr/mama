#!/usr/bin/env bash

set -e

mkdir -p build
cd build
cmake .. -DMAMA_TESTING=ON
make -j4 mavka