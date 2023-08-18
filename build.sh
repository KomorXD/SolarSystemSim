#!/usr/bin/bash

configuration="debug"

if [ $# -eq 0 ]; then
    echo "No configuration specified, 'debug' assumed."
else
    if [ "$1" = "debug" ] || [ $1 = "d" ]; then
        echo "Building for debug"
        configuration="debug"
    elif [ "$1" = "release" ] || [ $1 = "r" ]; then
        echo "Building for release"
        configuration="release"
    elif [ "$1" = "prod" ] || [ $1 = "p" ]; then
        echo "Building for production"
        configuration="prod"
    else
        echo "Config must be in a [ debug, release, prod ] set. Debug assumed"
        configuration="debug"
    fi
fi

cmake --preset "x64-$configuration-linux"
cmake --build "out/build/x64-$configuration-linux"
ln -s -f "out/build/x64-$configuration-linux/compile_commands.json" "compile_commands.json"
