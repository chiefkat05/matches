#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "system.h"

void level_start_timer(void *g, void *a, void *pr);
void level_timer_unlink(void *g, void *a, void *pr);
void level_destroy_object(void *g, void *a, void *pr);
void level_change_input(void *g, void *a, void *pr);
void save_input_data(void *g, void *a, void *pr);

void set_fullscreen(void *g, void *a, void *pr);
void set_windowed(void *g, void *a, void *pr);
void set_quit(void *g, void *a, void *pr);

int random_int(int min, int max)
{
    return (int)((rand() / (RAND_MAX * 1.0)) * (max - min) + min);
}
void level_start_timer(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    struct timer *pTimer = (struct timer *)pr;

    timer_start(pTimer, g, a);
}
void set_fullscreen(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    struct application *pApp = (struct application *)a;
    SDL_SetWindowFullscreen(pApp->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
}
void set_windowed(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    struct application *pApp = (struct application *)a;
    SDL_SetWindowFullscreen(pApp->window, 0);
}
void set_quit(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    pGame->running = false;
}
void level_timer_unlink(void *g, void *a, void *pr)
{
    struct timer *pTimer = (struct timer *)pr;
    timer_unlink(pTimer);
}
void level_destroy_object(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    struct visual *pObj = (struct visual *)pr;

    game_destroy_visual(pGame, pObj);
}
void level_change_input(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    enum input_action_type action = (enum input_action_type)pr;

    if (pGame->input_action_being_changed != action)
        game_change_input(pGame, action);
    else if (pGame->input_action_being_changed == action)
        game_change_input(pGame, INPUT_ACTION_NONE);
}

#endif