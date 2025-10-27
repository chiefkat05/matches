#ifndef DATA_H
#define DATA_H

#include "system.h"

// game functions
void randomDingSound(void *g, void *a, unsigned int index)
{
    struct game *pGame = (struct game *)g;
    int rand_gen_sound = random_int(0, 5);
    int rand_gen_volume = random_int(30, 60);
    game_set_sound_volume(g, rand_gen_sound, rand_gen_volume);
    game_play_sound(g, rand_gen_sound);
}
void newRandomPosition(struct game *g, unsigned int index)
{
    int rand_gen_x = random_int(0, WINDOW_WIDTH - 100);
    int rand_gen_y = random_int(0, WINDOW_HEIGHT - 200);
    g->screen.objects[index].rect.x = rand_gen_x;
    g->screen.objects[index].rect.y = rand_gen_y;
}
void level_start_timer(void *g, void *a, unsigned int index)
{
    struct game *pGame = (struct game *)g;
    struct application *pApp = (struct application *)a;

    timer_start(&pGame->timers[index], g, a);
}

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
    SDL_SetWindowFullscreen(pApp->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
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
    game_load_font(g, a, "./scabber-font/Scabber-q2Mn0.ttf", 64);
    game_add_texture(g, a, "./graphics/game/bg-1.png");
    game_add_text(g, a, 100, 100, "Play", OBJ_RESPONSE_CLICKED, set_level_clicker, 0);
    game_add_text(g, a, WINDOW_WIDTH - 280, WINDOW_HEIGHT - 80, "Quit", OBJ_RESPONSE_CLICKED, set_quit, 1);

    game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, OBJ_RESPONSE_NONE, NULL, 0);
}

void level_clicker_loop(struct game *g, struct application *a);

void level_clicker(struct game *g, struct application *a)
{
    game_load_font(g, a, "./scabber-font/Scabber-q2Mn0.ttf", 32);
    game_add_texture(g, a, "./graphics/game/idfk.png");
    game_add_texture(g, a, "./graphics/game/bg-1.png");
    game_add_visual(g, 0, 0, 1280, 720, 1, OBJ_RESPONSE_NONE, NULL, 0);

    int smallcircletimer = game_add_timer(g, 255.0, 0.0, 10.0, true);
    int mediumcircletimer = game_add_timer(g, 255.0, 0.0, 10.0, true);
    int largecircletimer = game_add_timer(g, 255.0, 0.0, 10.0, true);

    int smallcircle = game_add_visual(g, 100, 100, 10, 10, 0, OBJ_RESPONSE_CLICKED, level_start_timer, smallcircletimer);
    int mediumcircle = game_add_visual(g, 250, 250, 50, 50, 0, OBJ_RESPONSE_CLICKED, level_start_timer, mediumcircletimer);
    int largecircle = game_add_visual(g, 500, 500, 100, 100, 0, OBJ_RESPONSE_CLICKED, level_start_timer, largecircletimer);

    game_timer_link_int8(g, smallcircletimer, &g->screen.objects[smallcircle].color.a);
    game_timer_add_start_func(g, smallcircletimer, randomDingSound, 0);
    game_timer_add_func(g, smallcircletimer, updateScore10, smallcircle);

    game_timer_link_int8(g, mediumcircletimer, &g->screen.objects[mediumcircle].color.a);
    game_timer_add_start_func(g, mediumcircletimer, randomDingSound, 0);
    game_timer_add_func(g, mediumcircletimer, updateScore3, mediumcircle);

    game_timer_link_int8(g, largecircletimer, &g->screen.objects[largecircle].color.a);
    game_timer_add_start_func(g, largecircletimer, randomDingSound, 0);
    game_timer_add_func(g, largecircletimer, updateScore1, largecircle);

    game_add_text(g, a, 10, WINDOW_HEIGHT - 40, "Score: 0", OBJ_RESPONSE_NONE, NULL, 0);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 80, "Fullscreen", OBJ_RESPONSE_CLICKED, set_fullscreen, 1);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 120, "Windowed", OBJ_RESPONSE_CLICKED, set_windowed, 2);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 240, "Menu", OBJ_RESPONSE_CLICKED, set_level_menu, 3);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 40, "Quit", OBJ_RESPONSE_CLICKED, set_quit, 4);

    game_set_music(g, "./audio/music/dull.ogg");
    game_add_sound(g, "./audio/sfx/collect_1.wav");
    game_add_sound(g, "./audio/sfx/collect_2.wav");
    game_add_sound(g, "./audio/sfx/collect_3.wav");
    game_add_sound(g, "./audio/sfx/collect_4.wav");
    game_add_sound(g, "./audio/sfx/collect_5.wav");

    game_play_music(g);

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