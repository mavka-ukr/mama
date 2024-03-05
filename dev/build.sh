#!/usr/bin/env bash

set -e

mkdir -p build
cd build
cmake .. -DMAMA_DEV_MAVKA=ON
make -j4 mavka