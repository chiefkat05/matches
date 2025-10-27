echo 'building for linux'
x86_64-w64-mingw32-gcc main.c -o run -I/usr/include/SDL2/ `pkg-config --cflags --libs sdl2 SDL2_ttf SDL2_image SDL2_mixer`