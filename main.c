#include "system.h"

struct application app;
struct game g;

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>

bool main_initiated = false;
void main_loop()
{
    if (!main_initiated)
    {
        game_init(&g, &app);
        main_initiated = true;
    }
    game_loop(&g, &app);
}
#endif

int main()
{
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, false);
#endif
#ifndef __EMSCRIPTEN__
    game_init(&g, &app);

    while (game_loop(&g, &app))
    {
    }

    game_exit(&g, &app);
#endif
}