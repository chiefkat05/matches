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

#define INPUT_BUFFER_LIMIT 8

#define VISUAL_ANIMATION_FRAME_LIMIT 32
#define VISUAL_ANIMATION_SPEED 1.0

#define JOYSTICK_DEVICE_LIMIT 8
#define JOYSTICK_INPUT_CODE_LIMIT 16
#define JOYSTICK_AXIS_LIMIT 8
#define JOYSTICK_HAT_LIMIT 4
#define JOYSTICK_BALL_LIMIT 0 // maybe some day
#define JOYSTICK_BUTTON_LIMIT 16
#define JOYSTICK_AXIS_DEADZONE 3200 // make this configurable!!!
// also please if possible make the joystick input just a little less convoluted

enum object_special_type
{
    OBJECT_TYPE_NONE = 0,
    OBJECT_TYPE_BUTTON = 1,
    OBJECT_TYPE_GENERIC_2D_PLAYER = 2,
    OBJECT_TYPE_COLLISION = 4
};

enum input_action_type
{
    INPUT_ACTION_NONE,
    INPUT_ACTION_RIGHT,
    INPUT_ACTION_LEFT,
    INPUT_ACTION_UP,
    INPUT_ACTION_DOWN,
    INPUT_ACTION_INTERACT,
    input_action_type_limit
};
enum input_type
{
    INPUT_KEYBOARD,
    INPUT_JOYSTICK
};

#define JOYSTICK_AXIS_VALUE_TYPE_COUNT 2
#define JOYSTICK_HAT_VALUE_TYPE_COUNT 4
enum joystick_input_code
{
    JOYSTICK_AXIS_0_MIN,
    JOYSTICK_AXIS_0_MAX,
    JOYSTICK_AXIS_1_MIN,
    JOYSTICK_AXIS_1_MAX,
    JOYSTICK_AXIS_2_MIN,
    JOYSTICK_AXIS_2_MAX,
    JOYSTICK_AXIS_3_MIN,
    JOYSTICK_AXIS_3_MAX,
    JOYSTICK_AXIS_4_MIN,
    JOYSTICK_AXIS_4_MAX,
    JOYSTICK_AXIS_5_MIN,
    JOYSTICK_AXIS_5_MAX,
    JOYSTICK_AXIS_6_MIN,
    JOYSTICK_AXIS_6_MAX,
    JOYSTICK_AXIS_7_MIN,
    JOYSTICK_AXIS_7_MAX,

    JOYSTICK_HAT_0_NORTH,
    JOYSTICK_HAT_0_EAST,
    JOYSTICK_HAT_0_SOUTH,
    JOYSTICK_HAT_0_WEST,
    JOYSTICK_HAT_1_NORTH,
    JOYSTICK_HAT_1_EAST,
    JOYSTICK_HAT_1_SOUTH,
    JOYSTICK_HAT_1_WEST,
    JOYSTICK_HAT_2_NORTH,
    JOYSTICK_HAT_2_EAST,
    JOYSTICK_HAT_2_SOUTH,
    JOYSTICK_HAT_2_WEST,
    JOYSTICK_HAT_3_NORTH,
    JOYSTICK_HAT_3_EAST,
    JOYSTICK_HAT_3_SOUTH,
    JOYSTICK_HAT_3_WEST,

    JOYSTICK_BUTTON_0,
    JOYSTICK_BUTTON_1,
    JOYSTICK_BUTTON_2,
    JOYSTICK_BUTTON_3,
    JOYSTICK_BUTTON_4,
    JOYSTICK_BUTTON_5,
    JOYSTICK_BUTTON_6,
    JOYSTICK_BUTTON_7,
    JOYSTICK_BUTTON_8,
    JOYSTICK_BUTTON_9,
    JOYSTICK_BUTTON_10,
    JOYSTICK_BUTTON_11,
    JOYSTICK_BUTTON_12,
    JOYSTICK_BUTTON_13,
    JOYSTICK_BUTTON_14,
    JOYSTICK_BUTTON_15,

    joystick_input_limit
};
struct joystick_input
{
    enum joystick_input_code code;
    int joystick_id;
};
struct joystick_default_map
{
    SDL_Joystick *joystick;
    int joystick_axis_values[JOYSTICK_AXIS_LIMIT];
};
struct input_scancode
{
    uint8_t code;
    enum input_type type;
};
struct input_action_data
{
    enum input_action_type type;
    struct input_scancode scancodes[INPUT_BUFFER_LIMIT];
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
    struct joystick_default_map joysticks[JOYSTICK_DEVICE_LIMIT];
    struct joystick_input joystick_input_changed_this_frame[INPUT_BUFFER_LIMIT];
    bool joystick_input_map[joystick_input_limit * JOYSTICK_DEVICE_LIMIT];
    unsigned int joystick_input_count;

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