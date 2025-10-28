echo 'building for linux'
mkdir -p ./linbuild/
rm -rf ./linbuild/graphics/
rm -rf ./linbuild/audio/
gcc main.c -o ./linbuild/run `pkg-config --cflags --libs sdl2 SDL2_ttf SDL2_image SDL2_mixer` -O3
cp -r ./graphics/ ./linbuild/
cp -r ./audio/ ./linbuild/