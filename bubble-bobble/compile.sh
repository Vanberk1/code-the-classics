#! /bin/sh

cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build

if [ "$1" = "-run" ] || [ "$1" = "-r" ]; then
    shift
    ./rungame.sh "$@"
fi

exit 0