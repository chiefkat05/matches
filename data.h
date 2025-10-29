#ifndef DATA_H
#define DATA_H

#include "system.h"

// generic functions
int random_int(int min, int max);
void level_start_timer(void *g, void *a, void *pr);
void level_timer_unlink(void *g, void *a, void *pr);
void level_destroy_object(void *g, void *a, void *pr);

void set_fullscreen(void *g, void *a, void *pr);
void set_windowed(void *g, void *a, void *pr);
void set_quit(void *g, void *a, void *pr);
void set_level_clicker(void *g, void *a, void *pr);
void set_level_menu(void *g, void *a, void *pr);

// level_clicker functions
void level_clicker_loop(struct game *g, struct application *a);
void create_clickable_circle(struct game *g, struct application *a, int xpos, int ypos, int width, int height, void (*func)(void *g, void *a, void *pr));
void updateScore1(void *g, void *a, void *pr);
void randomDingSound(void *g, void *a, void *pr);
void spawnCircle(void *g, void *a, void *pr);
void newRandomPosition(struct game *g, void *pr);

// features I want:
// a z-index for objects and text that determines draw order (higher number means draw later)
// make one big clickable circle that always gives points
// make a single shop button that spends score in exchange for multi increase
// make the shop button price go up over time
// congratulations you have cookie clicker
// object response when player pressed a move direction
// some control in data over what inputs are considered what type of action (gamepad dpad up + W key + Up key + mouse move up = MOVE_UP for example)
// add a little character to move around when you get high enough score (like 200)

// ignorable bonus: some control over the non-func trigger interactions of objects (like the mousehover reaction of clickable visuals)
//  why the above is a bad idea: the visual-func system right now is a very simple and understandable one object + one interaction = one thing that happens system
//  if you try too hard to allow customization of the less important things you might uproot this and make using the engine miserable
//  hard-coded eye candy is better than dynamic eye candy that takes forever to implement

void level_menu(struct game *g, struct application *a)
{
    game_load_font(g, a, "./graphics/fonts/scabber-font/Scabber-q2Mn0.ttf", 64);
    struct colored_texture *bgTexture = game_add_texture(g, a, "./graphics/game/bg-1.png");

    struct timer *quit_timer = game_add_timer(g, 0.0, 255.0, 10.0, false);
    struct timer *play_timer = game_add_timer(g, 0.0, 255.0, 10.0, false);
    game_add_text(g, a, 100, 100, 0, "Play", OBJ_RESPONSE_CLICKED, level_start_timer, play_timer);
    game_add_text(g, a, WINDOW_WIDTH - 280, WINDOW_HEIGHT - 80, 0, "Quit", OBJ_RESPONSE_CLICKED, level_start_timer, quit_timer);
    game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, -1, bgTexture, OBJ_RESPONSE_NONE, NULL, NULL);

    struct colored_texture *fadeTexture = game_add_texture(g, a, "./graphics/game/fade.png");
    struct visual *transitionfade = game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 1, fadeTexture, OBJ_RESPONSE_NONE, NULL, NULL);
    visual_set_color(transitionfade, 1.0, 1.0, 1.0, 0.0);
    game_timer_link_int8(g, play_timer, &transitionfade->color.a);
    game_timer_set_func(g, play_timer, set_level_clicker, NULL);
    game_timer_link_int8(g, quit_timer, &transitionfade->color.a);
    game_timer_set_func(g, quit_timer, set_quit, NULL);

    struct timer *fadeintimer = game_add_timer(g, 255.0, 0.0, 10.0, false);
    game_timer_link_int8(g, fadeintimer, &transitionfade->color.a);

    level_start_timer(g, a, fadeintimer);
}

struct level_clicker_data_struct
{
    struct colored_texture *circleTexture;
    struct visual *scoreText;
    int circleCount;
    struct timer *circleAppearTimer;
    struct timer *circleAppearAlphaTimer;
    int score;
    struct visual *transitionfade;
};
struct level_clicker_data_struct level_clicker_data;
void level_clicker(struct game *g, struct application *a)
{
    game_load_font(g, a, "./graphics/fonts/scabber-font/Scabber-q2Mn0.ttf", 32);
    level_clicker_data.circleTexture = game_add_texture(g, a, "./graphics/game/idfk.png");
    struct colored_texture *bgTexture = game_add_texture(g, a, "./graphics/game/bg-1.png");
    struct colored_texture *fadeTexture = game_add_texture(g, a, "./graphics/game/fade.png");
    game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, -1, bgTexture, OBJ_RESPONSE_NONE, NULL, NULL);

    struct timer *quit_timer = game_add_timer(g, 0.0, 255.0, 10.0, false);
    struct timer *menu_timer = game_add_timer(g, 0.0, 255.0, 10.0, false);

    level_clicker_data.transitionfade = game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 1, fadeTexture, OBJ_RESPONSE_NONE, NULL, NULL);
    visual_set_color(level_clicker_data.transitionfade, 1.0, 1.0, 1.0, 0.0);

    level_clicker_data.scoreText = game_add_text(g, a, 10, WINDOW_HEIGHT - 40, 0, "Score: 0", OBJ_RESPONSE_NONE, NULL, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 80, 0, "Fullscreen", OBJ_RESPONSE_CLICKED, set_fullscreen, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 120, 0, "Windowed", OBJ_RESPONSE_CLICKED, set_windowed, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 240, 0, "Menu", OBJ_RESPONSE_CLICKED, level_start_timer, menu_timer);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 40, 0, "Quit", OBJ_RESPONSE_CLICKED, level_start_timer, quit_timer);

    game_timer_link_int8(g, menu_timer, &level_clicker_data.transitionfade->color.a);
    game_timer_set_func(g, menu_timer, set_level_menu, NULL);
    game_timer_link_int8(g, quit_timer, &level_clicker_data.transitionfade->color.a);
    game_timer_set_func(g, quit_timer, set_quit, NULL);

    struct timer *fadeintimer = game_add_timer(g, 255.0, 0.0, 10.0, false);
    game_timer_link_int8(g, fadeintimer, &level_clicker_data.transitionfade->color.a);

    level_start_timer(g, a, fadeintimer);

    level_clicker_data.circleAppearTimer = game_add_timer(g, 10.0, 0.0, 0.1, true);
    game_timer_set_func(g, level_clicker_data.circleAppearTimer, spawnCircle, NULL);

    level_clicker_data.circleAppearAlphaTimer = game_add_timer(g, 0.0, 255.0, 10.0, true);
    game_timer_set_func(g, level_clicker_data.circleAppearAlphaTimer, level_timer_unlink, NULL);

    game_set_music(g, "./audio/music/dull.ogg");
    game_add_sound(g, "./audio/sfx/collect_1.wav");
    game_add_sound(g, "./audio/sfx/collect_2.wav");
    game_add_sound(g, "./audio/sfx/collect_3.wav");
    game_add_sound(g, "./audio/sfx/collect_4.wav");
    game_add_sound(g, "./audio/sfx/collect_5.wav");

    game_play_music(g);

    game_set_level_loop_func(g, level_clicker_loop);

    level_clicker_data.score = 0;
}
void level_clicker_loop(struct game *g, struct application *a)
{
    char scorestr[TEXT_BUFFER_CAP] = "Score: ";
    char input[sizeof(long)];
    sprintf(input, "%d", level_clicker_data.score);
    strcat(scorestr, input);
    game_change_text(g, a, level_clicker_data.scoreText, scorestr);

    if (g->game_time > 0.0)
    {
        level_start_timer(g, a, level_clicker_data.circleAppearTimer);
    }
}

// game functions
int random_int(int min, int max)
{
    return (int)((rand() / (RAND_MAX * 1.0)) * (max - min) + min);
}
void set_level_clicker(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    struct application *pApp = (struct application *)a;
    game_change_level(pGame, pApp, level_clicker);
}
void set_level_menu(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    struct application *pApp = (struct application *)a;
    game_change_level(pGame, pApp, level_menu);
}

void randomDingSound(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    int rand_gen_sound = random_int(0, 5);
    int rand_gen_volume = random_int(30, 60);
    game_set_sound_volume(pGame, pGame->audio.sounds[rand_gen_sound], rand_gen_volume);
    game_play_sound(pGame, pGame->audio.sounds[rand_gen_sound]);
}
void newRandomPosition(struct game *g, void *pr)
{
    struct visual *pObj = (struct visual *)pr;
    int rand_gen_x = random_int(0, WINDOW_WIDTH - 400);
    int rand_gen_y = random_int(0, WINDOW_HEIGHT - 200);
    pObj->rect.x = rand_gen_x;
    pObj->rect.y = rand_gen_y;
}
void spawnCircle(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;

    create_clickable_circle(pGame, a, 0, 0, 100, 100, updateScore1);
}
void level_start_timer(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    struct timer *pTimer = (struct timer *)pr;

    timer_start(pTimer, g, a);
}

void updateScore1(void *g, void *a, void *pr)
{
    struct visual *pObj = (struct visual *)pr;
    visual_set_interaction(pObj, OBJ_RESPONSE_NONE, NULL, NULL);
    randomDingSound(g, a, pr);
    ++level_clicker_data.score;
    --level_clicker_data.circleCount;
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

void create_clickable_circle(struct game *g, struct application *a, int xpos, int ypos, int width, int height, void (*func)(void *g, void *a, void *pr))
{
    if (level_clicker_data.circleCount > 8)
    {
        return;
    }

    struct timer *timer = game_add_timer(g, 255.0, 0.0, 10.0, true);

    struct visual *circle = game_add_visual(g, xpos, ypos, width, height, 0, level_clicker_data.circleTexture, OBJ_RESPONSE_CLICKED, level_start_timer, timer);
    if (level_clicker_data.circleAppearAlphaTimer != NULL && level_clicker_data.circleAppearAlphaTimer->pLinkInt8 == NULL)
    {
        game_timer_link_int8(g, level_clicker_data.circleAppearAlphaTimer, &circle->color.a);
        level_start_timer(g, a, level_clicker_data.circleAppearAlphaTimer);
    }

    game_timer_link_int8(g, timer, &circle->color.a);
    game_timer_set_start_func(g, timer, func, circle);
    game_timer_set_func(g, timer, level_destroy_object, circle);
    newRandomPosition(g, circle);
    ++level_clicker_data.circleCount;
}

#endif