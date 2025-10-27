echo 'building for linux'
gcc main.c -o run `pkg-config --cflags --libs sdl2 SDL2_ttf SDL2_image SDL2_mixer`