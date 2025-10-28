#ifndef DATA_H
#define DATA_H

#include "system.h"

// game functions
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
    int rand_gen_x = random_int(0, WINDOW_WIDTH - 100);
    int rand_gen_y = random_int(0, WINDOW_HEIGHT - 200);
    pObj->rect.x = rand_gen_x;
    pObj->rect.y = rand_gen_y;
}
void level_start_timer(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    struct application *pApp = (struct application *)a;
    struct timer *pTimer = (struct timer *)pr;

    timer_start(pTimer, g, a);
}

void updateScore1(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    newRandomPosition(pGame, pr);
    pGame->score += 1;
}
void updateScore3(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    newRandomPosition(pGame, pr);
    pGame->score += 3;
}
void updateScore10(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    newRandomPosition(pGame, pr);
    pGame->score += 10;
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
void set_level_clicker(void *g, void *a, void *pr);
void set_level_menu(void *g, void *a, void *pr);

void level_clicker_loop(struct game *g, struct application *a);

struct colored_texture *circleTexture;
void create_clickable_circle(struct game *g, int xpos, int ypos, int width, int height, void (*func)(void *g, void *a, void *pr))
{
    struct timer *timer = game_add_timer(g, 255.0, 0.0, 10.0, true);

    struct visual *circle = game_add_visual(g, xpos, ypos, width, height, circleTexture, OBJ_RESPONSE_CLICKED, level_start_timer, timer);

    game_timer_link_int8(g, timer, &circle->color.a);
    game_timer_add_start_func(g, timer, randomDingSound, NULL);
    game_timer_add_func(g, timer, func, circle);
}

// features I want:
// ability to destroy objects (possibly necessary to lessen the grip of the int index things in your game struct. Replace with pointers?)
// ability to create objects during loop (these 'features' may already be implement-able but I want confirmation that they can be done)
// make the clickable circles actually appear every so often using g->game_time and then slowly disappear
// make one big clickable circle that always gives points
// a game struct-compatible level_data structure??? that can help instead of having global variables for say score multi and stuff??? maybe this is stupider than just global variables???
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
    game_add_text(g, a, 100, 100, "Play", OBJ_RESPONSE_CLICKED, set_level_clicker, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 280, WINDOW_HEIGHT - 80, "Quit", OBJ_RESPONSE_CLICKED, set_quit, NULL);

    game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, bgTexture, OBJ_RESPONSE_NONE, NULL, NULL);
}

struct text *scoreText;
void level_clicker(struct game *g, struct application *a)
{
    game_load_font(g, a, "./graphics/fonts/scabber-font/Scabber-q2Mn0.ttf", 32);
    circleTexture = game_add_texture(g, a, "./graphics/game/idfk.png");
    struct colored_texture *bgTexture = game_add_texture(g, a, "./graphics/game/bg-1.png");
    game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, bgTexture, OBJ_RESPONSE_NONE, NULL, NULL);

    create_clickable_circle(g, 500, 500, 100, 100, updateScore1);
    create_clickable_circle(g, 1000, 500, 100, 100, updateScore1);

    struct timer *quit_timer = game_add_timer(g, 0.0, 255.0, 10.0, false);
    struct timer *menu_timer = game_add_timer(g, 0.0, 255.0, 10.0, false);

    scoreText = game_add_text(g, a, 10, WINDOW_HEIGHT - 40, "Score: 0", OBJ_RESPONSE_NONE, NULL, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 80, "Fullscreen", OBJ_RESPONSE_CLICKED, set_fullscreen, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 120, "Windowed", OBJ_RESPONSE_CLICKED, set_windowed, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 240, "Menu", OBJ_RESPONSE_CLICKED, level_start_timer, menu_timer);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 40, "Quit", OBJ_RESPONSE_CLICKED, level_start_timer, quit_timer);

    struct visual *transitionfade = game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, circleTexture, OBJ_RESPONSE_NONE, NULL, NULL);
    visual_set_color(transitionfade, 1.0, 1.0, 1.0, 0.0);

    game_timer_link_int8(g, menu_timer, &transitionfade->color.a);
    game_timer_add_func(g, menu_timer, set_level_menu, NULL);
    game_timer_link_int8(g, quit_timer, &transitionfade->color.a);
    game_timer_add_func(g, quit_timer, set_quit, NULL);

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
    char scorestr[TEXT_BUFFER_CAP] = "Score: ";
    char input[sizeof(long)];
    sprintf(input, "%d", g->score);
    strcat(scorestr, input);
    game_change_text(g, a, scoreText, scorestr);

    // if (g->game_time > 1.0)
    // {
    //     game_destroy_visual(g, &g->screen.objects[1]);
    // }
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

#endif