#! /bin/sh

if [ "$1" = "-gdb" ] || [ "$1" = "-g" ]; then
    gdb ./build/pong.exe
else
    ./build/pong.exe
fi

exit 0