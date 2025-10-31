#ifndef DATA_H
#define DATA_H

#include "functions.h"

// generic functions (at some point please move all these to their own header file called functions.h or something to avoid bloat in the main game data header xoxoxo)
void set_level_1(void *g, void *a, void *pr);
void set_level_menu(void *g, void *a, void *pr);

// level_1 functions
void level_1_loop(struct game *g, struct application *a);
void create_clickable_circle(struct game *g, struct application *a, int xpos, int ypos, int width, int height, void (*func)(void *g, void *a, void *pr));
void updateScore1(void *g, void *a, void *pr);
void randomDingSound(void *g, void *a, void *pr);
void randomStepSound(void *g, void *a, void *pr);
void spawnCircle(void *g, void *a, void *pr);
void newRandomPosition(struct game *g, void *pr);
void animatePlayer(void *g, void *a, void *pr);

void level_menu(struct game *g, struct application *a)
{
    game_load_font(g, a, "./graphics/fonts/scabber-font/Scabber-q2Mn0.ttf", 64);
    struct colored_texture *bgTexture = game_add_texture(g, a, "./graphics/game/bg-1.png");

    struct timer *quit_timer = game_add_timer(g, 0.0, 255.0, 10.0, false);
    struct timer *play_timer = game_add_timer(g, 0.0, 255.0, 10.0, false);
    game_add_text(g, a, 100, 100, 0, "Play", OBJECT_TYPE_BUTTON, level_start_timer, play_timer);
    game_add_text(g, a, WINDOW_WIDTH - 280, WINDOW_HEIGHT - 80, 0, "Quit", OBJECT_TYPE_BUTTON, level_start_timer, quit_timer);
    game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, -1, bgTexture, OBJECT_TYPE_NONE, NULL, NULL);

    struct colored_texture *fadeTexture = game_add_texture(g, a, "./graphics/game/fade.png");
    struct visual *transitionfade = game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 1, fadeTexture, OBJECT_TYPE_NONE, NULL, NULL);
    visual_set_color(transitionfade, 1.0, 1.0, 1.0, 0.0);
    game_timer_link_int8(g, play_timer, &transitionfade->color.a);
    game_timer_set_func(g, play_timer, set_level_1, NULL);
    game_timer_link_int8(g, quit_timer, &transitionfade->color.a);
    game_timer_set_func(g, quit_timer, set_quit, NULL);

    struct timer *fadeintimer = game_add_timer(g, 255.0, 0.0, 10.0, false);
    game_timer_link_int8(g, fadeintimer, &transitionfade->color.a);

    level_start_timer(g, a, fadeintimer);
}

struct level_1_data_struct
{
    struct colored_texture *circleTexture;
    struct visual *scoreText;
    int circleCount;
    struct timer *circleAppearTimer;
    struct timer *circleAppearAlphaTimer;
    int score;
    struct visual *transitionfade;
    struct visual *player;
    struct visual *downinputtext;
    struct visual *upinputtext;
    struct visual *rightinputtext;
    struct visual *leftinputtext;

    struct visual_animation playerAnim;
};
struct level_1_data_struct level_1_data;

void level_1(struct game *g, struct application *a)
{
    game_load_font(g, a, "./graphics/fonts/scabber-font/Scabber-q2Mn0.ttf", 32);
    level_1_data.circleTexture = game_add_texture(g, a, "./graphics/game/idfk.png");
    struct colored_texture *bgTexture = game_add_texture(g, a, "./graphics/game/bg-1.png");
    struct colored_texture *fadeTexture = game_add_texture(g, a, "./graphics/game/fade.png");
    struct colored_texture *dudeTexture = game_add_texture(g, a, "./graphics/game/dude.png");
    game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, -1, bgTexture, OBJECT_TYPE_NONE, NULL, NULL);

    struct timer *quit_timer = game_add_timer(g, 0.0, 255.0, 10.0, false);
    struct timer *menu_timer = game_add_timer(g, 0.0, 255.0, 10.0, false);

    level_1_data.player = game_add_visual(g, 100, 100, 100, 100, 1, dudeTexture, OBJECT_TYPE_GENERIC_2D_PLAYER, animatePlayer, NULL);
    visual_set_frames(level_1_data.player, 2, 1);
    v_animation_init(&level_1_data.playerAnim, level_1_data.player);
    v_animation_add_frame(&level_1_data.playerAnim, 10.0, 0, NULL, NULL);
    v_animation_add_frame(&level_1_data.playerAnim, 20.0, 1, randomStepSound, NULL);

    game_add_input(g, INPUT_ACTION_DOWN, INPUT_KEYBOARD, SDL_SCANCODE_S);
    game_add_input(g, INPUT_ACTION_UP, INPUT_KEYBOARD, SDL_SCANCODE_W);
    game_add_input(g, INPUT_ACTION_LEFT, INPUT_KEYBOARD, SDL_SCANCODE_A);
    game_add_input(g, INPUT_ACTION_RIGHT, INPUT_KEYBOARD, SDL_SCANCODE_D);
    game_add_input(g, INPUT_ACTION_DOWN, INPUT_KEYBOARD, SDL_SCANCODE_DOWN);
    game_add_input(g, INPUT_ACTION_UP, INPUT_KEYBOARD, SDL_SCANCODE_UP);
    game_add_input(g, INPUT_ACTION_LEFT, INPUT_KEYBOARD, SDL_SCANCODE_LEFT);
    game_add_input(g, INPUT_ACTION_RIGHT, INPUT_KEYBOARD, SDL_SCANCODE_RIGHT);

    level_1_data.transitionfade = game_add_visual(g, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 100, fadeTexture, OBJECT_TYPE_NONE, NULL, NULL);
    visual_set_color(level_1_data.transitionfade, 1.0, 1.0, 1.0, 0.0);

    level_1_data.scoreText = game_add_text(g, a, 10, WINDOW_HEIGHT - 40, 0, "Score: 0", OBJECT_TYPE_NONE, NULL, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 80, 0, "Fullscreen", OBJECT_TYPE_BUTTON, set_fullscreen, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 120, 0, "Windowed", OBJECT_TYPE_BUTTON, set_windowed, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 240, 0, "Menu", OBJECT_TYPE_BUTTON, level_start_timer, menu_timer);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 40, 0, "Quit", OBJECT_TYPE_BUTTON, level_start_timer, quit_timer);

    level_1_data.upinputtext = game_add_text(g, a, 20, WINDOW_HEIGHT - 120, 0, "Up Key: ", OBJECT_TYPE_BUTTON, level_change_input, (void *)INPUT_ACTION_UP);
    level_1_data.leftinputtext = game_add_text(g, a, 20, WINDOW_HEIGHT - 160, 0, "Left Key: ", OBJECT_TYPE_BUTTON, level_change_input, (void *)INPUT_ACTION_LEFT);
    level_1_data.rightinputtext = game_add_text(g, a, 20, WINDOW_HEIGHT - 200, 0, "Right Key: ", OBJECT_TYPE_BUTTON, level_change_input, (void *)INPUT_ACTION_RIGHT);
    level_1_data.downinputtext = game_add_text(g, a, 20, WINDOW_HEIGHT - 80, 0, "Down Key: ", OBJECT_TYPE_BUTTON, level_change_input, (void *)INPUT_ACTION_DOWN);

    game_timer_link_int8(g, menu_timer, &level_1_data.transitionfade->color.a);
    game_timer_set_func(g, menu_timer, set_level_menu, NULL);
    game_timer_link_int8(g, quit_timer, &level_1_data.transitionfade->color.a);
    game_timer_set_func(g, quit_timer, set_quit, NULL);

    struct timer *fadeintimer = game_add_timer(g, 255.0, 0.0, 10.0, false);
    game_timer_link_int8(g, fadeintimer, &level_1_data.transitionfade->color.a);

    level_start_timer(g, a, fadeintimer);

    level_1_data.circleAppearTimer = game_add_timer(g, 10.0, 0.0, 0.1, true);
    game_timer_set_func(g, level_1_data.circleAppearTimer, spawnCircle, NULL);

    level_1_data.circleAppearAlphaTimer = game_add_timer(g, 0.0, 255.0, 10.0, true);
    game_timer_set_func(g, level_1_data.circleAppearAlphaTimer, level_timer_unlink, NULL);

    game_set_music(g, "./audio/music/dull.ogg");
    game_add_sound(g, "./audio/sfx/collect_1.wav");
    game_add_sound(g, "./audio/sfx/collect_2.wav");
    game_add_sound(g, "./audio/sfx/collect_3.wav");
    game_add_sound(g, "./audio/sfx/collect_4.wav");
    game_add_sound(g, "./audio/sfx/collect_5.wav");
    game_add_sound(g, "./audio/sfx/step_1.wav");
    game_add_sound(g, "./audio/sfx/step_2.wav");
    game_add_sound(g, "./audio/sfx/step_3.wav");
    game_add_sound(g, "./audio/sfx/step_4.wav");

    game_play_music(g);

    game_set_level_loop_func(g, level_1_loop);

    level_1_data.score = 0;
    load_file_data("./game_data/score", &level_1_data.score, sizeof(int), 0);
    load_file_data("./game_data/inputdefaults", &g->input_actions[INPUT_ACTION_DOWN], sizeof(struct input_action_data), INPUT_ACTION_DOWN);
    load_file_data("./game_data/inputdefaults", &g->input_actions[INPUT_ACTION_UP], sizeof(struct input_action_data), INPUT_ACTION_UP);
    load_file_data("./game_data/inputdefaults", &g->input_actions[INPUT_ACTION_LEFT], sizeof(struct input_action_data), INPUT_ACTION_LEFT);
    load_file_data("./game_data/inputdefaults", &g->input_actions[INPUT_ACTION_RIGHT], sizeof(struct input_action_data), INPUT_ACTION_RIGHT);
    load_file_data("./game_data/inputdefaults", &g->input_actions[INPUT_ACTION_INTERACT], sizeof(struct input_action_data), INPUT_ACTION_INTERACT);
}
void level_1_loop(struct game *g, struct application *a)
{
    static int old_score = 0;
    v_animation_update(&level_1_data.playerAnim, g, a);

    char scorestr[TEXT_BUFFER_CAP] = "Score: ";
    char input[sizeof(long)];
    sprintf(input, "%d", level_1_data.score);
    strcat(scorestr, input);
    game_change_text(g, a, level_1_data.scoreText, scorestr);
    if (level_1_data.score != old_score)
    {
        old_score = level_1_data.score;
        save_file_data("./game_data/score", &level_1_data.score, sizeof(int), 1);
    }

    char downinputstr[TEXT_BUFFER_CAP] = "Down Key: ";
    strcat(downinputstr, game_get_action_key_names_list(g, INPUT_ACTION_DOWN));
    game_change_text(g, a, level_1_data.downinputtext, downinputstr);
    char upinputstr[TEXT_BUFFER_CAP] = "Up Key: ";
    strcat(upinputstr, game_get_action_key_names_list(g, INPUT_ACTION_UP));
    game_change_text(g, a, level_1_data.upinputtext, upinputstr);
    char leftinputstr[TEXT_BUFFER_CAP] = "Left Key: ";
    strcat(leftinputstr, game_get_action_key_names_list(g, INPUT_ACTION_LEFT));
    game_change_text(g, a, level_1_data.leftinputtext, leftinputstr);
    char rightinputstr[TEXT_BUFFER_CAP] = "Right Key: ";
    strcat(rightinputstr, game_get_action_key_names_list(g, INPUT_ACTION_RIGHT));
    game_change_text(g, a, level_1_data.rightinputtext, rightinputstr);

    game_do_on_input_change(g, a, level_1_data.rightinputtext, INPUT_ACTION_RIGHT, save_input_data);
    game_do_on_input_change(g, a, level_1_data.downinputtext, INPUT_ACTION_DOWN, save_input_data);
    game_do_on_input_change(g, a, level_1_data.upinputtext, INPUT_ACTION_UP, save_input_data);
    game_do_on_input_change(g, a, level_1_data.leftinputtext, INPUT_ACTION_LEFT, save_input_data);
    visual_set_color(level_1_data.upinputtext, 1.0, 1.0, 1.0, 1.0);
    visual_set_color(level_1_data.leftinputtext, 1.0, 1.0, 1.0, 1.0);
    visual_set_color(level_1_data.rightinputtext, 1.0, 1.0, 1.0, 1.0);
    visual_set_color(level_1_data.downinputtext, 1.0, 1.0, 1.0, 1.0);
    if (g->input_action_being_changed == INPUT_ACTION_DOWN)
    {
        visual_set_color(level_1_data.downinputtext, 0.2, 1.0, 0.2, 1.0);
    }
    if (g->input_action_being_changed == INPUT_ACTION_UP)
    {
        visual_set_color(level_1_data.upinputtext, 0.2, 1.0, 0.2, 1.0);
    }
    if (g->input_action_being_changed == INPUT_ACTION_LEFT)
    {
        visual_set_color(level_1_data.leftinputtext, 0.2, 1.0, 0.2, 1.0);
    }
    if (g->input_action_being_changed == INPUT_ACTION_RIGHT)
    {
        visual_set_color(level_1_data.rightinputtext, 0.2, 1.0, 0.2, 1.0);
    }

    if (g->game_time > 0.5)
    {
        level_start_timer(g, a, level_1_data.circleAppearTimer);
    }

    if (level_1_data.player->rect.x < 0)
    {
        level_1_data.player->rect.x = 0;
    }
    if (level_1_data.player->rect.x > WINDOW_WIDTH - level_1_data.player->rect.w)
    {
        level_1_data.player->rect.x = WINDOW_WIDTH - level_1_data.player->rect.w;
    }
    if (level_1_data.player->rect.y < 0)
    {
        level_1_data.player->rect.y = 0;
    }
    if (level_1_data.player->rect.y > WINDOW_HEIGHT - level_1_data.player->rect.h)
    {
        level_1_data.player->rect.y = WINDOW_HEIGHT - level_1_data.player->rect.h;
    }
}

void save_input_data(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    struct visual *pObj = (struct visual *)pr;

    save_file_data("./game_data/inputdefaults", &pGame->input_actions[0], sizeof(struct input_action_data), input_action_type_limit);
}
void animatePlayer(void *g, void *a, void *pr)
{
    v_animation_play(&level_1_data.playerAnim);
}
// game functions
int random_int(int min, int max)
{
    return (int)((rand() / (RAND_MAX * 1.0)) * (max - min) + min);
}
void set_level_1(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    struct application *pApp = (struct application *)a;
    game_change_level(pGame, pApp, level_1);
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
void randomStepSound(void *g, void *a, void *pr)
{
    struct game *pGame = (struct game *)g;
    int rand_gen_sound = random_int(5, 9);
    int rand_gen_volume = random_int(30, 60);
    game_set_sound_volume(pGame, pGame->audio.sounds[rand_gen_sound], rand_gen_volume);
    game_play_sound(pGame, pGame->audio.sounds[rand_gen_sound]);
}
void newRandomPosition(struct game *g, void *pr)
{
    struct visual *pObj = (struct visual *)pr;
    int rand_gen_x = random_int(0, WINDOW_WIDTH - 400);
    int rand_gen_y = random_int(0, WINDOW_HEIGHT - 300);
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
    visual_set_interaction(pObj, OBJECT_TYPE_NONE, NULL, NULL);
    randomDingSound(g, a, pr);
    ++level_1_data.score;
    --level_1_data.circleCount;
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
        game_change_input(g, action);
    else if (pGame->input_action_being_changed == action)
        game_change_input(g, INPUT_ACTION_NONE);
}

void create_clickable_circle(struct game *g, struct application *a, int xpos, int ypos, int width, int height, void (*func)(void *g, void *a, void *pr))
{
    if (level_1_data.circleCount > 8)
    {
        return;
    }

    struct timer *timer = game_add_timer(g, 255.0, 0.0, 10.0, true);

    struct visual *circle = game_add_visual(g, xpos, ypos, width, height, 0, level_1_data.circleTexture, OBJECT_TYPE_BUTTON, level_start_timer, timer);
    if (level_1_data.circleAppearAlphaTimer != NULL && level_1_data.circleAppearAlphaTimer->pLinkInt8 == NULL)
    {
        game_timer_link_int8(g, level_1_data.circleAppearAlphaTimer, &circle->color.a);
        level_start_timer(g, a, level_1_data.circleAppearAlphaTimer);
    }

    game_timer_link_int8(g, timer, &circle->color.a);
    game_timer_set_start_func(g, timer, func, circle);
    game_timer_set_func(g, timer, level_destroy_object, circle);
    newRandomPosition(g, circle);
    ++level_1_data.circleCount;
}

#endif