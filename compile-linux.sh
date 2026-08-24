#!/bin/bash

MODE=$1

if [ "$MODE" == "--force"]; then
    if [ -d "build" ]; then
        echo "Limpando a pasta build antiga..."
        rm -rf build
    fi
fi

cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=/usr/bin/g++-14

cmake --build build
