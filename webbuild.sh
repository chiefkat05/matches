echo 'building for web'
mkdir -p ./webbuild
emcc --use-port=sdl2 --use-port=sdl2_image:formats=png --use-port=sdl2_mixer --use-port=sdl2_ttf ./main.c -o ./webbuild/index.html -I/usr/include/SDL2/ -sALLOW_MEMORY_GROWTH -s DISABLE_EXCEPTION_CATCHING=1 -s ENVIRONMENT=web --preload-file graphics --preload-file audio --preload-file scabber-font