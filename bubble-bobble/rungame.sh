#! /bin/sh

if [ "$1" = "-gdb" ] || [ "$1" = "-g" ]; then
    gdb ./build/bubble-bobble.exe
else
    ./build/bubble-bobble.exe
fi

exit 0