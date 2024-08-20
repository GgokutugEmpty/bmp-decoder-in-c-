#!/bin/bash
SOURCE_FILE="bmpfile_decoder.c"
OUTPUT_BINARY="bmpfile_decoder"
SDL_CFLAGS=`sdl2-config --cflags`
SDL_LDFLAGS=`sdl2-config --libs`
gcc -o $OUTPUT_BINARY $SOURCE_FILE $SDL_CFLAGS $SDL_LDFLAGS
if [ $? -eq 0 ]; then
    echo "Compilation successful!"
    echo "You can run your program with ./$OUTPUT_BINARY"
else
    echo "Compilation failed."
    exit 1
fi
