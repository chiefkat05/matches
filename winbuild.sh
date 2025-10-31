echo 'building for windows'
mkdir -p ./winbuild
rm -rf ./winbuild/graphics/
rm -rf ./winbuild/audio/
rm -rf ./winbuild/game_data/
x86_64-w64-mingw32-gcc main.c -o ./winbuild/run.exe -I/usr/include/SDL2/ -O3 -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
cp -r ./graphics/ ./winbuild/
cp -r ./audio/ ./winbuild/
cp -r ./game_data/ ./winbuild/
cp -r ./buildetc/winlibs/*.dll ./winbuild/