#! /usr/bin/bash

mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=/home/alex/DeepShark/src/Cpp/inc/libtorch ..
cmake --build . --config Release