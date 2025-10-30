#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#define SDL_MAIN_HANDLED 1
#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_net.h>
#endif
#ifndef __linux__
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <SDL_net.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

#define SPECIAL_ANIMATION_SPEED 3
#define SPECIAL_ANIMATION_MIN 2

#define TEXTURE_CAP 24
#define OBJECT_CAP 128
#define SOUND_CAP 48
#define SIMULTANEOUS_SOUND_CAP 12
#define TEXT_BUFFER_CAP 128
#define TIMER_OBJECT_CAP 128

#define INPUT_VALID_SCANCODE_LIMIT 8

#define VISUAL_ANIMATION_FRAME_LIMIT 32
#define VISUAL_ANIMATION_SPEED 1.0

enum object_special_type
{
    OBJECT_TYPE_NONE = 0,
    OBJECT_TYPE_BUTTON = 1,
    OBJECT_TYPE_GENERIC_2D_PLAYER = 2,
    OBJECT_TYPE_COLLISION = 4
};

enum input_action_type
{
    INPUT_NONE,
    INPUT_RIGHT,
    INPUT_LEFT,
    INPUT_UP,
    INPUT_DOWN,
    INPUT_INTERACT,
    input_action_type_limit
};
struct input_action_data
{
    enum input_action_type type;
    uint8_t valid_scancodes[INPUT_VALID_SCANCODE_LIMIT];
    unsigned int scancode_count;
    bool held, just_pressed, just_released;
};

struct timer
{
    double current_time;
    double start_time;
    double end_time;
    double speed;
    bool running;
    bool restart_on_end;

    int timer_direction;

    double *pLink;
    uint8_t *pLinkInt8;

    void *func_pRef;
    void (*func)(void *g, void *a, void *pRef);

    void *start_func_pRef;
    void (*start_func)(void *g, void *a, void *pRef);
};
struct colored_texture
{
    SDL_Texture *texture;
    SDL_Color color;
};

struct visual
{
    SDL_Rect rect;
    int z_index;

    unsigned int x_frames, y_frames;
    SDL_Rect animation_rect;
    int sx, sy;
    int sw, sh;
    char *text_str; // make this into just a char *

    int real_sx, real_sy;
    int real_sw, real_sh;

    SDL_Color color;

    struct colored_texture *pTexture;
    int object_type_flags;

    void *func_pRef;
    void (*func)(void *game, void *app, void *pRef);
};
struct graphic_layer
{
    struct colored_texture clr_textures[TEXTURE_CAP];
    unsigned int texture_count;

    struct visual *objects[OBJECT_CAP];
    unsigned int object_count;

    TTF_Font *font;
};
struct audio_bank
{
    Mix_Chunk *sounds[SOUND_CAP];
    unsigned int sound_count;
    Mix_Music *music;
};
struct visual_animation_frame
{
    double end_time;
    int pos_index;
    void (*func)(void *g, void *a, void *pr);
    void *func_pRef;
};
struct visual_animation
{
    struct visual *pVisual;
    struct visual_animation_frame *frames[VISUAL_ANIMATION_FRAME_LIMIT];
    unsigned int frame_count;
    double time;
    unsigned int current_frame;

    bool running;
};

struct application
{
    SDL_Renderer *renderer;
    SDL_Window *window;
};
struct game
{
    struct timer timers[TIMER_OBJECT_CAP];
    unsigned int timer_count;

    struct input_action_data input_actions[input_action_type_limit];

    struct graphic_layer screen;
    struct audio_bank audio;
    bool running, level_initiated, need_z_reorder;
    int mouseX, mouseY;
    bool mousePressed, mouseClicked, mouseReleased;
    double game_time;
    double window_x_scale;
    double window_y_scale;
    void (*level_loop_func)(struct game *g, struct application *a);

    enum input_action_type input_action_being_changed, input_action_just_changed;
};

#endif