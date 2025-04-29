#! /bin/bash

cmake -S . -B build -G Ninja -DCMAKE_C_COMPILER=clang-19 -DCMAKE_CXX_COMPILER=clang++-19 -DCMAKE_CXX_FLAGS="-stdlib=libc++"
