#ifndef SYSTEM_H
#define SYSTEM_H

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
#define TEXT_OBJECT_CAP 14
#define TIMER_OBJECT_CAP 128

int random_int(int min, int max)
{
    return (int)((rand() / (RAND_MAX * 1.0)) * (max - min) + min);
}

// set these to real bit Doubles??
enum object_response_flags
{
    OBJ_RESPONSE_NONE = 0,
    OBJ_RESPONSE_CLICKED = 1,
    OBJ_RESPONSE_DRAGGED = 2
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

    unsigned int func_index;
    void (*func)(void *g, void *a, unsigned int i);

    unsigned int start_func_index;
    void (*start_func)(void *g, void *a, unsigned int i);
};
void timer_start(struct timer *t, void *g, void *a)
{
    if (t->running)
        return;

    t->running = true;
    t->current_time = t->start_time;
    if (t->start_func != NULL)
    {
        t->start_func(g, a, t->start_func_index);
    }
}
void timer_update(struct timer *t, void *g, void *a)
{
    if (!t->running)
    {
        if (t->pLink != NULL)
            *t->pLink = t->current_time;
        if (t->pLinkInt8 != NULL)
            *t->pLinkInt8 = (uint8_t)t->current_time;
        return;
    }
    if (t->timer_direction == 1)
    {
        t->current_time += t->speed;
    }
    if (t->timer_direction == -1)
    {
        t->current_time -= t->speed;
    }

    // timer finished chedck for normal timer direction
    if (t->timer_direction == -1)
    {
        if (t->current_time < t->end_time || t->current_time == t->end_time)
        {
            if (!t->restart_on_end)
                t->current_time = t->end_time;
            if (t->restart_on_end)
                t->current_time = t->start_time;

            t->running = false;

            if (t->func != NULL)
            {
                t->func(g, a, t->func_index);
            }
        }
    }
    if (t->timer_direction == 1)
    {
        if (t->current_time > t->end_time || t->current_time == t->end_time)
        {
            if (!t->restart_on_end)
                t->current_time = t->end_time;
            if (t->restart_on_end)
                t->current_time = t->start_time;

            t->running = false;

            if (t->func != NULL)
            {
                t->func(g, a, t->func_index);
            }
        }
    }

    if (t->pLink != NULL)
        *t->pLink = t->current_time;
    if (t->pLinkInt8 != NULL)
        *t->pLinkInt8 = (uint8_t)t->current_time;
}

struct visual
{
    SDL_Rect rect;
    int sx, sy;
    int sw, sh;

    int real_sx, real_sy;
    int real_sw, real_sh;

    SDL_Color color;

    unsigned int texture;
    enum object_response_flags response_flags;

    unsigned int func_index;
    void (*func)(void *game, void *app, unsigned int i);
};
void visual_set_color(struct visual *v, double r, double g, double b, double a)
{
    SDL_Color new_clr;
    new_clr.r = r * 255.0;
    new_clr.g = g * 255.0;
    new_clr.b = b * 255.0;
    new_clr.a = a * 255.0;
    v->color = new_clr;
}
void visual_set_special(struct visual *v, int x, int y, int w, int h)
{
    v->sx = x;
    v->sy = y;
    v->sw = w;
    v->sh = h;
}
void visual_set_special_size(struct visual *v, int w, int h)
{
    v->sx = -(w / 2);
    v->sy = -(h / 2);
    v->sw = w;
    v->sh = h;
}
void visual_update_special(struct visual *v)
{
    if (v->real_sx != v->sx)
    {
        v->real_sx += ((v->sx - v->real_sx) / SPECIAL_ANIMATION_SPEED);
        if (abs(v->sx - v->real_sx) < SPECIAL_ANIMATION_MIN)
        {
            v->real_sx = v->sx;
        }
    }
    if (v->real_sy != v->sy)
    {
        v->real_sy += ((v->sy - v->real_sy) / SPECIAL_ANIMATION_SPEED);
        if (abs(v->sy - v->real_sy) < SPECIAL_ANIMATION_MIN)
        {
            v->real_sy = v->sy;
        }
    }
    if (v->real_sw != v->sw)
    {
        v->real_sw += ((v->sw - v->real_sw) / SPECIAL_ANIMATION_SPEED);
        if (abs(v->sw - v->real_sw) < SPECIAL_ANIMATION_MIN)
        {
            v->real_sw = v->sw;
        }
    }
    if (v->real_sh != v->sh)
    {
        v->real_sh += ((v->sh - v->real_sh) / SPECIAL_ANIMATION_SPEED);
        if (abs(v->sh - v->real_sh) < SPECIAL_ANIMATION_MIN)
        {
            v->real_sh = v->sh;
        }
    }
}
struct text
{
    struct visual graphic;
    SDL_Texture *texture;
    char text[100];
};

struct graphic_layer
{
    SDL_Texture *textures[TEXTURE_CAP];
    SDL_Color texture_colors[TEXTURE_CAP];
    unsigned int texture_count;

    struct visual objects[OBJECT_CAP];
    unsigned int object_count;
    struct text text_objects[TEXT_OBJECT_CAP];
    unsigned int text_object_count;

    TTF_Font *font;
};
struct audio_bank
{
    Mix_Chunk *sounds[SOUND_CAP];
    unsigned int sound_count;
    Mix_Music *music;
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

    struct graphic_layer screen;
    struct audio_bank audio;
    bool running, level_initiated;
    int mouseX, mouseY;
    bool mousePressed, mouseClicked, mouseReleased;
    double game_time;
    int score;
    double window_x_scale;
    double window_y_scale;
    void (*level_loop_func)(struct game *g, struct application *a);
};

bool mouse_collision(struct game *g, struct visual *v)
{
    SDL_Rect temp_rect = v->rect;
    temp_rect.x *= g->window_x_scale;
    temp_rect.w *= g->window_x_scale;
    temp_rect.y *= g->window_y_scale;
    temp_rect.h *= g->window_y_scale;
    if (g->mouseX < temp_rect.x || g->mouseX > temp_rect.x + temp_rect.w ||
        g->mouseY < temp_rect.y || g->mouseY > temp_rect.y + temp_rect.h)
        return false;
    return true;
}

void texture_set_color(SDL_Texture *t, SDL_Color clr)
{
    SDL_SetTextureColorMod(t, clr.r, clr.g, clr.b);
    SDL_SetTextureAlphaMod(t, clr.a);
}

int game_add_timer(struct game *g, double start_time, double end_time, double speed, bool roe)
{
    if (g->timer_count >= TIMER_OBJECT_CAP)
    {
        printf("Error too many timers\n");
        return -1;
    }

    g->timers[g->timer_count].start_time = start_time;
    g->timers[g->timer_count].current_time = start_time;
    g->timers[g->timer_count].end_time = end_time;
    g->timers[g->timer_count].speed = speed;

    g->timers[g->timer_count].running = false;
    g->timers[g->timer_count].restart_on_end = roe;

    g->timers[g->timer_count].pLink = NULL;
    g->timers[g->timer_count].pLinkInt8 = NULL;

    g->timers[g->timer_count].timer_direction = -1;
    if (start_time < end_time)
        g->timers[g->timer_count].timer_direction = 1;

    ++g->timer_count;

    return g->timer_count - 1;
}
void game_timer_add_func(struct game *g, unsigned int t_index, void (*func)(void *g, void *a, unsigned int i), unsigned int func_i)
{
    g->timers[t_index].func_index = func_i;
    g->timers[t_index].func = func;
}
void game_timer_link(struct game *g, unsigned int t_index, double *pL)
{
    g->timers[t_index].pLink = pL;
}
void game_timer_link_int8(struct game *g, unsigned int t_index, uint8_t *pL)
{
    g->timers[t_index].pLinkInt8 = pL;
}
void game_timer_add_start_func(struct game *g, unsigned int t_index, void (*func)(void *g, void *a, unsigned int i), unsigned int func_i)
{
    g->timers[t_index].start_func = func;
    g->timers[t_index].start_func_index = func_i;
}

int game_add_texture(struct game *g, struct application *a, const char *path)
{
    if (g->screen.texture_count >= TEXTURE_CAP)
    {
        printf("Error too many textures\n");
        return -1;
    }
    SDL_Surface *surf = IMG_Load(path);
    if (!surf)
    {
        printf("Error IMG_Load() failure %s\n", IMG_GetError());
        return -1;
    }
    g->screen.textures[g->screen.texture_count] = SDL_CreateTextureFromSurface(a->renderer, surf);
    if (!g->screen.textures[g->screen.texture_count])
    {
        printf("Error SDL_CreateTextureFromSurface() failure %s\n", SDL_GetError());
        return -1;
    }
    SDL_FreeSurface(surf);

    ++g->screen.texture_count;

    return g->screen.texture_count - 1;
}
int game_add_visual(struct game *g, int x, int y, int w, int h, unsigned int t,
                    enum object_response_flags rf, void (*func)(void *game, void *app, unsigned int i), unsigned int index)
{
    if (g->screen.object_count >= OBJECT_CAP)
    {
        printf("Error too many objects\n");
        return -1;
    }
    if (t > g->screen.texture_count)
    {
        printf("Error requested texture %i does not exist\n", t);
        return -1;
    }
    g->screen.objects[g->screen.object_count].rect.x = x;
    g->screen.objects[g->screen.object_count].rect.y = y;
    g->screen.objects[g->screen.object_count].rect.w = w;
    g->screen.objects[g->screen.object_count].rect.h = h;
    g->screen.objects[g->screen.object_count].texture = t;
    g->screen.objects[g->screen.object_count].response_flags = rf;
    g->screen.objects[g->screen.object_count].func = func;
    g->screen.objects[g->screen.object_count].func_index = index;
    SDL_Color clr;
    clr.r = 255;
    clr.g = 255;
    clr.b = 255;
    clr.a = 255;
    g->screen.objects[g->screen.object_count].color = clr;
    ++g->screen.object_count;

    return g->screen.object_count - 1;
}
void game_change_text(struct game *g, struct application *a, unsigned int text_index, char updated_text[TEXT_BUFFER_CAP])
{
    if (text_index >= g->screen.text_object_count)
    {
        printf("Error text_index for change text does not exist\n");
        return;
    }
    const char *t1 = g->screen.text_objects[text_index].text;
    const char *t2 = updated_text;
    if (strcmp(t1, t2) == 0)
        return;
    strcpy(g->screen.text_objects[text_index].text, updated_text);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(g->screen.font, g->screen.text_objects[text_index].text, white);
    if (!surfaceMessage)
    {
        printf("Error TTF_RenderText_Solid() failure\n");
        return;
    }

    g->screen.text_objects[text_index].texture = SDL_CreateTextureFromSurface(a->renderer, surfaceMessage);
    if (!g->screen.text_objects[text_index].texture)
    {
        printf("Error SDL_CreateTextureFromSurface() failure\n");
        return;
    }

    TTF_SizeText(g->screen.font, g->screen.text_objects[text_index].text, &g->screen.text_objects[text_index].graphic.rect.w, &g->screen.text_objects[text_index].graphic.rect.h);
}
void game_load_font(struct game *g, struct application *a, const char *path, unsigned int size)
{
    g->screen.font = TTF_OpenFont(path, size);
    if (!g->screen.font)
    {
        printf("Error TTF_OpenFont failure() %s\n", TTF_GetError());
        return;
    }
}
int game_add_text(struct game *g, struct application *a, int x, int y, char *t,
                  enum object_response_flags rf, void (*func)(void *game, void *app, unsigned int i), unsigned int index)
{
    if (g->screen.font == NULL)
    {
        printf("Error called game_add_text() with no font loaded\n");
        return -1;
    }
    if (g->screen.text_object_count >= TEXT_OBJECT_CAP)
    {
        printf("Error too much text\n");
        return -1;
    }
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(g->screen.font, t, white);
    if (!surfaceMessage)
    {
        printf("Error TTF_RenderText_Solid() failure\n");
        return -1;
    }

    g->screen.text_objects[g->screen.text_object_count].texture = SDL_CreateTextureFromSurface(a->renderer, surfaceMessage);
    if (!g->screen.text_objects[g->screen.text_object_count].texture)
    {
        printf("Error SDL_CreateTextureFromSurface() failure\n");
        return -1;
    }

    g->screen.text_objects[g->screen.text_object_count].graphic.rect.x = x;
    g->screen.text_objects[g->screen.text_object_count].graphic.rect.y = y;
    TTF_SizeText(g->screen.font, t, &g->screen.text_objects[g->screen.text_object_count].graphic.rect.w, &g->screen.text_objects[g->screen.text_object_count].graphic.rect.h);
    strcpy(g->screen.text_objects[g->screen.text_object_count].text, t);

    g->screen.text_objects[g->screen.text_object_count].graphic.rect.x = x;
    g->screen.text_objects[g->screen.text_object_count].graphic.rect.y = y;
    g->screen.text_objects[g->screen.text_object_count].graphic.response_flags = rf;
    g->screen.text_objects[g->screen.text_object_count].graphic.func = func;
    g->screen.text_objects[g->screen.object_count].graphic.func_index = index;
    ++g->screen.text_object_count;

    return g->screen.text_object_count - 1;
}
void game_set_music(struct game *g, const char *path)
{
    g->audio.music = Mix_LoadMUS(path);
    if (!g->audio.music)
    {
        printf("Error Mix_LoadMUS() failure %s\n", Mix_GetError());
        return;
    }
}
void game_play_music(struct game *g)
{
    Mix_PlayMusic(g->audio.music, 1);
}
int game_add_sound(struct game *g, const char *path)
{
    if (g->audio.sound_count >= SOUND_CAP)
    {
        printf("Error too many sounds\n");
        return -1;
    }
    g->audio.sounds[g->audio.sound_count] = Mix_LoadWAV(path);
    if (!g->audio.sounds[g->audio.sound_count])
    {
        printf("Error Mix_LoadWAV() failure %s\n", Mix_GetError());
        return -1;
    }
    ++g->audio.sound_count;

    return g->audio.sound_count - 1;
}
void game_set_sound_volume(struct game *g, unsigned int index, int volume)
{
    if (index > g->audio.sound_count)
    {
        printf("Error sound at index %i does not exist\n", index);
        return;
    }
    g->audio.sounds[index]->volume = volume;
}
void game_play_sound(struct game *g, unsigned int index)
{
    if (index > g->audio.sound_count)
    {
        printf("Error sound at index %i does not exist\n", index);
        return;
    }
    Mix_PlayChannel(-1, g->audio.sounds[index], 0);
}
void game_set_level_loop_func(struct game *g, void (*func)(struct game *g, struct application *a))
{
    g->level_loop_func = func;
}

bool colors_different(SDL_Color clr1, SDL_Color clr2)
{
    if (clr1.r != clr2.r || clr1.g != clr2.g || clr1.b != clr2.b || clr1.a != clr2.a)
        return true;
    return false;
}
void game_draw(struct game *g, struct application *a)
{
    for (int i = 0; i < g->screen.object_count; ++i)
    {
        if (g->screen.objects[i].texture < 0)
            continue;

        if (colors_different(g->screen.objects[i].color, g->screen.texture_colors[g->screen.objects[i].texture]))
        {
            SDL_Color clr1 = g->screen.objects[i].color;
            SDL_Color clr2 = g->screen.texture_colors[g->screen.objects[i].texture];
            texture_set_color(g->screen.textures[g->screen.objects[i].texture], g->screen.objects[i].color);
            g->screen.texture_colors[g->screen.objects[i].texture] = g->screen.objects[i].color;
        }

        SDL_Rect temp_rect = g->screen.objects[i].rect;
        temp_rect.x += g->screen.objects[i].real_sx;
        temp_rect.y += g->screen.objects[i].real_sy;
        temp_rect.w += g->screen.objects[i].real_sw;
        temp_rect.h += g->screen.objects[i].real_sh;

        temp_rect.x *= g->window_x_scale;
        temp_rect.w *= g->window_x_scale;
        temp_rect.y *= g->window_y_scale;
        temp_rect.h *= g->window_y_scale;
        SDL_RenderCopy(a->renderer, g->screen.textures[g->screen.objects[i].texture], NULL, &temp_rect);
    }
    for (int i = 0; i < g->screen.text_object_count; ++i)
    {
        if (g->screen.text_objects[i].texture < 0)
            continue;

        SDL_Rect temp_rect = g->screen.text_objects[i].graphic.rect;
        temp_rect.x += g->screen.text_objects[i].graphic.real_sx;
        temp_rect.y += g->screen.text_objects[i].graphic.real_sy;
        temp_rect.w += g->screen.text_objects[i].graphic.real_sw;
        temp_rect.h += g->screen.text_objects[i].graphic.real_sh;

        temp_rect.x *= g->window_x_scale;
        temp_rect.w *= g->window_x_scale;
        temp_rect.y *= g->window_y_scale;
        temp_rect.h *= g->window_y_scale;
        SDL_RenderCopy(a->renderer, g->screen.text_objects[i].texture, NULL, &temp_rect);
    }
}

void game_init(struct game *g, struct application *a)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("Error SDL_Init() failure %s\n", SDL_GetError());
        return;
    }

    a->window = SDL_CreateWindow("Content", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    g->window_x_scale = 1.0;
    g->window_y_scale = 1.0;

    if (!a->window)
    {
        printf("Error, SDL_CreateWindow() failure\n");
        return;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    a->renderer = SDL_CreateRenderer(a->window, -1, SDL_RENDERER_ACCELERATED);

    if (!a->renderer)
    {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("Error IMG_Init() failure\n");
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("Error Mix_OpenAudio() failure\n");
        return;
    }
    Mix_AllocateChannels(SIMULTANEOUS_SOUND_CAP + 1);

    if (TTF_Init() < 0)
    {
        printf("Error TTF_Init() failure %s\n", TTF_GetError());
        return;
    }

    g->level_initiated = false;
    g->running = true;
    g->game_time = 0.0;
    g->score = 0;
}
void game_build_level(struct game *g, struct application *a, void (*func)(struct game *g, struct application *a))
{
    if (func == NULL)
    {
        printf("Error no function provided to game_build_level(), can't build level\n");
    }
    func(g, a);
    g->level_initiated = true;
}
void game_destroy_level(struct game *g, struct application *a)
{
    for (int i = 0; i < g->screen.object_count; ++i)
    {
        g->screen.objects[i].rect.x = 0.0;
        g->screen.objects[i].rect.y = 0.0;
        g->screen.objects[i].rect.w = 0.0;
        g->screen.objects[i].rect.h = 0.0;
        g->screen.objects[i].func = NULL;
        g->screen.objects[i].response_flags = 0;
        g->screen.objects[i].texture = -1;
        g->screen.objects[i].sx = 0.0;
        g->screen.objects[i].sy = 0.0;
        g->screen.objects[i].sw = 0.0;
        g->screen.objects[i].sh = 0.0;
        g->screen.objects[i].real_sx = 0.0;
        g->screen.objects[i].real_sy = 0.0;
        g->screen.objects[i].real_sw = 0.0;
        g->screen.objects[i].real_sh = 0.0;
    }
    g->screen.object_count = 0;
    for (int i = 0; i < g->screen.texture_count; ++i)
    {
        if (g->screen.textures[i] != NULL)
        {
            SDL_DestroyTexture(g->screen.textures[i]);
            g->screen.textures[i] = NULL;
        }
    }
    g->screen.texture_count = 0;
    if (g->screen.font != NULL)
    {
        SDL_free(g->screen.font);
        g->screen.font = NULL;
    }
    for (int i = 0; i < g->screen.text_object_count; ++i)
    {
        strcpy(g->screen.text_objects[i].text, "");
        if (g->screen.text_objects[i].texture != NULL)
        {
            SDL_DestroyTexture(g->screen.text_objects[i].texture);
            g->screen.text_objects[i].texture = NULL;
        }

        g->screen.text_objects[i].graphic.rect.x = 0.0;
        g->screen.text_objects[i].graphic.rect.y = 0.0;
        g->screen.text_objects[i].graphic.rect.w = 0.0;
        g->screen.text_objects[i].graphic.rect.h = 0.0;
        g->screen.text_objects[i].graphic.func = NULL;
        g->screen.text_objects[i].graphic.response_flags = 0;
        g->screen.text_objects[i].graphic.texture = -1;
        g->screen.text_objects[i].graphic.sx = 0.0;
        g->screen.text_objects[i].graphic.sy = 0.0;
        g->screen.text_objects[i].graphic.sw = 0.0;
        g->screen.text_objects[i].graphic.sh = 0.0;
        g->screen.text_objects[i].graphic.real_sx = 0.0;
        g->screen.text_objects[i].graphic.real_sy = 0.0;
        g->screen.text_objects[i].graphic.real_sw = 0.0;
        g->screen.text_objects[i].graphic.real_sh = 0.0;
    }
    g->screen.text_object_count = 0;

    if (g->audio.music != NULL)
    {
        Mix_FreeMusic(g->audio.music);
        g->audio.music = NULL;
    }
    for (int i = 0; i < g->audio.sound_count; ++i)
    {
        if (g->audio.sounds[i] != NULL)
        {
            Mix_FreeChunk(g->audio.sounds[i]);
            g->audio.sounds[i] = NULL;
        }
    }
    g->audio.sound_count = 0;
    g->level_loop_func = NULL;

    g->level_initiated = false;
}
void game_change_level(struct game *g, struct application *a, void (*func)(struct game *g, struct application *a))
{
    game_destroy_level(g, a);
    game_build_level(g, a, func);
}

void game_exit(struct game *g, struct application *a)
{
    game_destroy_level(g, a);
    SDL_DestroyRenderer(a->renderer);
    SDL_DestroyWindow(a->window);
}

void draw_loop(struct game *g, struct application *a);
void event_loop(struct game *g);
void logic_loop(struct game *g, struct application *a);

bool game_loop(struct game *g, struct application *a)
{
    g->game_time += 0.01;
    SDL_SetRenderDrawColor(a->renderer, 255, 255, 255, 255);
    SDL_RenderClear(a->renderer);

    event_loop(g);
    if (g->level_initiated)
    {
        logic_loop(g, a);
        if (g->level_loop_func != NULL)
        {
            g->level_loop_func(g, a);
        }

        draw_loop(g, a);
    }

    SDL_Delay(1000 / 60);

    if (g->running == false)
    {
        return false;
    }

    return true;
}
void logic_loop(struct game *g, struct application *a)
{
    // objects
    for (int i = 0; i < g->screen.object_count; ++i)
    {
        visual_update_special(&g->screen.objects[i]);
        visual_set_special(&g->screen.objects[i], 0, 0, 0, 0);

        if (g->screen.objects[i].response_flags & (1 << 0))
        {
            if (mouse_collision(g, &g->screen.objects[i]))
            {
                visual_set_special_size(&g->screen.objects[i], g->screen.objects[i].rect.w * 0.25, g->screen.objects[i].rect.h * 0.25);
                if (g->mousePressed)
                {
                    visual_set_special_size(&g->screen.objects[i], g->screen.objects[i].rect.w * -0.25, g->screen.objects[i].rect.h * -0.25);
                }
                if (g->mouseClicked)
                {
                    visual_set_special_size(&g->screen.objects[i], g->screen.objects[i].rect.w * 2, g->screen.objects[i].rect.h * 2);
                    if (g->screen.objects[i].func != NULL)
                    {
                        g->screen.objects[i].func(g, a, g->screen.objects[i].func_index);
                    }
                }
            }
        }
    }

    // text (please combine this with objects somehow)
    for (int i = 0; i < g->screen.text_object_count; ++i)
    {
        visual_update_special(&g->screen.text_objects[i].graphic);
        visual_set_special(&g->screen.text_objects[i].graphic, 0, 0, 0, 0);

        if (g->screen.text_objects[i].graphic.response_flags & (1 << 0))
        {
            if (mouse_collision(g, &g->screen.text_objects[i].graphic))
            {
                visual_set_special_size(&g->screen.text_objects[i].graphic, g->screen.text_objects[i].graphic.rect.w * 0.25, g->screen.text_objects[i].graphic.rect.h * 0.25);
                if (g->mousePressed)
                {
                    visual_set_special_size(&g->screen.text_objects[i].graphic, g->screen.text_objects[i].graphic.rect.w * -0.25, g->screen.text_objects[i].graphic.rect.h * -0.25);
                }
                if (g->mouseClicked)
                {
                    visual_set_special_size(&g->screen.text_objects[i].graphic, g->screen.text_objects[i].graphic.rect.w * 2, g->screen.text_objects[i].graphic.rect.h * 2);
                    if (g->screen.text_objects[i].graphic.func != NULL)
                    {
                        g->screen.text_objects[i].graphic.func(g, a, g->screen.text_objects[i].graphic.func_index);
                    }
                }
            }
        }
    }

    for (int i = 0; i < g->timer_count; ++i)
    {
        timer_update(&g->timers[i], g, a);
    }
}

void draw_loop(struct game *g, struct application *a)
{
    game_draw(g, a);

    SDL_RenderPresent(a->renderer);
}
void event_loop(struct game *g)
{
    g->mouseClicked = false;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            g->running = false;
            break;
        case SDL_MOUSEMOTION:
            g->mouseX = event.motion.x;
            g->mouseY = event.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            g->mousePressed = true;
            g->mouseReleased = false;
            break;
        case SDL_MOUSEBUTTONUP:
            g->mouseReleased = true;
            g->mousePressed = false;
            g->mouseClicked = true;
            break;
        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                g->window_x_scale = ((double)event.window.data1 / (double)WINDOW_WIDTH);
                g->window_y_scale = ((double)event.window.data2 / (double)WINDOW_HEIGHT);
            }
            break;
        default:
            break;
        }
    }
}

#endif