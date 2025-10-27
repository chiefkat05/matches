#ifndef DATA_H
#define DATA_H

#include "system.h"

void updateScore1(void *g, void *a, unsigned int index)
{
    struct game *pGame = (struct game *)g;
    newRandomPosition(pGame, index);
    pGame->score += 1;
}
void updateScore3(void *g, void *a, unsigned int index)
{
    struct game *pGame = (struct game *)g;
    newRandomPosition(pGame, index);
    pGame->score += 3;
}
void updateScore10(void *g, void *a, unsigned int index)
{
    struct game *pGame = (struct game *)g;
    newRandomPosition(pGame, index);
    pGame->score += 10;
}
void set_fullscreen(void *g, void *a, unsigned int index)
{
    struct game *pGame = (struct game *)g;
    struct application *pApp = (struct application *)a;
    SDL_SetWindowFullscreen(pApp->window, SDL_WINDOW_FULLSCREEN);
}
void set_windowed(void *g, void *a, unsigned int index)
{
    struct game *pGame = (struct game *)g;
    struct application *pApp = (struct application *)a;
    SDL_SetWindowFullscreen(pApp->window, 0);
}
void set_quit(void *g, void *a, unsigned int index)
{
    struct game *pGame = (struct game *)g;
    pGame->running = false;
}
void set_level_clicker(void *g, void *a, unsigned int index);
void set_level_menu(void *g, void *a, unsigned int index);

void level_menu(struct game *g, struct application *a)
{
    game_load_font(g, a, "./scabber-font/Scabber-q2Mn0.ttf", 32); // I want to see why the error message doesn't appear for add_text, maybe in add_texture??
    game_add_texture(g, a, "./graphics/game/bg-1.png");           // I want to see why the level text 'play' is changed
    game_add_text(g, a, 100, 100, "Play", OBJ_RESPONSE_CLICKED, set_level_clicker);

    game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, OBJ_RESPONSE_NONE, NULL);
}

void level_clicker_loop(struct game *g, struct application *a);

void level_clicker(struct game *g, struct application *a)
{
    game_load_font(g, a, "./scabber-font/Scabber-q2Mn0.ttf", 32);
    game_add_texture(g, a, "./graphics/game/idfk.png");
    game_add_texture(g, a, "./graphics/game/bg-1.png");
    game_add_visual(g, 0, 0, 1280, 720, 1, OBJ_RESPONSE_NONE, NULL);
    game_add_visual(g, 100, 100, 10, 10, 0, OBJ_RESPONSE_CLICKED, updateScore10);
    game_add_visual(g, 250, 250, 50, 50, 0, OBJ_RESPONSE_CLICKED, updateScore3);
    game_add_visual(g, 500, 500, 100, 100, 0, OBJ_RESPONSE_CLICKED, updateScore1);

    game_add_text(g, a, 10, WINDOW_HEIGHT - 40, "Score: 0", OBJ_RESPONSE_NONE, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 80, "Fullscreen", OBJ_RESPONSE_CLICKED, set_fullscreen);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 120, "Windowed", OBJ_RESPONSE_CLICKED, set_windowed);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 240, "Menu", OBJ_RESPONSE_CLICKED, set_level_menu);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 40, "Quit", OBJ_RESPONSE_CLICKED, set_quit);

    game_set_music(g, "./audio/music/dull.ogg");
    game_add_sound(g, "./audio/sfx/collect_1.wav");
    game_add_sound(g, "./audio/sfx/collect_2.wav");
    game_add_sound(g, "./audio/sfx/collect_3.wav");
    game_add_sound(g, "./audio/sfx/collect_4.wav");
    game_add_sound(g, "./audio/sfx/collect_5.wav");

    game_play_music(g);
    game_play_sound(g, 0);

    game_set_level_loop_func(g, level_clicker_loop);
}
void level_clicker_loop(struct game *g, struct application *a)
{
    char scoretext[TEXT_BUFFER_CAP] = "Score: ";
    char input[sizeof(long)];
    sprintf(input, "%d", g->score);
    strcat(scoretext, input);
    game_change_text(g, a, 0, scoretext);
}

void set_level_clicker(void *g, void *a, unsigned int index)
{
    struct game *pGame = (struct game *)g;
    struct application *pApp = (struct application *)a;
    game_change_level(pGame, pApp, level_clicker);
}
void set_level_menu(void *g, void *a, unsigned int index)
{
    struct game *pGame = (struct game *)g;
    struct application *pApp = (struct application *)a;
    game_change_level(pGame, pApp, level_menu);
}

#endif