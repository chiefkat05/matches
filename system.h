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
enum object_response_trigger_types
{
    OBJ_RESPONSE_NONE,
    OBJ_RESPONSE_CLICKED,
    OBJ_RESPONSE_MOVE_RIGHT,
    OBJ_RESPONSE_MOVE_LEFT,
    OBJ_RESPONSE_MOVE_UP,
    OBJ_RESPONSE_MOVE_DOWN,
    OBJ_RESPONSE_MOVE_ACTION
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
void timer_start(struct timer *t, void *g, void *a)
{
    if (t->running)
        return;

    t->running = true;
    t->current_time = t->start_time;
    if (t->start_func != NULL)
    {
        t->start_func(g, a, t->start_func_pRef);
    }
}
void timer_update(struct timer *t, void *g, void *a)
{
    if (!t->running)
    {
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
                t->func(g, a, t->func_pRef);
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
                t->func(g, a, t->func_pRef);
            }
        }
    }

    if (t->pLink != NULL)
        *t->pLink = t->current_time;
    if (t->pLinkInt8 != NULL)
        *t->pLinkInt8 = (uint8_t)t->current_time;
}

struct colored_texture
{
    SDL_Texture *texture;
    SDL_Color color;
};

struct visual
{
    SDL_Rect rect;
    int sx, sy;
    int sw, sh;

    int real_sx, real_sy;
    int real_sw, real_sh;

    SDL_Color color;

    struct colored_texture *pTexture;
    enum object_response_trigger_types response_trigger;

    void *func_pRef;
    void (*func)(void *game, void *app, void *pRef);
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
void visual_destroy(struct visual *v)
{
    v->rect.x = 0.0;
    v->rect.y = 0.0;
    v->rect.w = 0.0;
    v->rect.h = 0.0;
    v->func = NULL;
    v->response_trigger = 0;
    v->pTexture = NULL;
    v->sx = 0.0;
    v->sy = 0.0;
    v->sw = 0.0;
    v->sh = 0.0;
    v->real_sx = 0.0;
    v->real_sy = 0.0;
    v->real_sw = 0.0;
    v->real_sh = 0.0;
}

struct text
{
    struct visual graphic;
    SDL_Texture *texture;
    char text[100];
};

struct graphic_layer
{
    struct colored_texture clr_textures[TEXTURE_CAP];
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

struct timer *game_add_timer(struct game *g, double start_time, double end_time, double speed, bool restart_on_end)
{
    if (g->timer_count >= TIMER_OBJECT_CAP)
    {
        printf("Error too many timers\n");
        return NULL;
    }

    g->timers[g->timer_count].start_time = start_time;
    g->timers[g->timer_count].current_time = start_time;
    g->timers[g->timer_count].end_time = end_time;
    g->timers[g->timer_count].speed = speed;

    g->timers[g->timer_count].running = false;
    g->timers[g->timer_count].restart_on_end = restart_on_end;

    g->timers[g->timer_count].pLink = NULL;
    g->timers[g->timer_count].pLinkInt8 = NULL;

    g->timers[g->timer_count].timer_direction = -1;
    if (start_time < end_time)
        g->timers[g->timer_count].timer_direction = 1;

    ++g->timer_count;

    return &g->timers[g->timer_count - 1];
}
void game_timer_add_func(struct game *g, struct timer *t, void (*func)(void *g, void *a, void *pr), void *func_pr)
{
    t->func = func;
    if (func_pr != NULL)
        t->func_pRef = func_pr;
    if (func_pr == NULL)
        t->func_pRef = t;
}
void game_timer_link(struct game *g, struct timer *t, double *pL)
{
    t->pLink = pL;
}
void game_timer_link_int8(struct game *g, struct timer *t, uint8_t *pL)
{
    t->pLinkInt8 = pL;
}
void game_timer_add_start_func(struct game *g, struct timer *t, void (*func)(void *g, void *a, void *pr), void *func_pr)
{
    t->start_func = func;
    if (func_pr != NULL)
        t->start_func_pRef = func_pr;
    if (func_pr == NULL)
        t->start_func_pRef = t;
}

struct colored_texture *game_add_texture(struct game *g, struct application *a, const char *path)
{
    if (g->screen.texture_count >= TEXTURE_CAP)
    {
        printf("Error too many textures\n");
        return NULL;
    }
    SDL_Surface *surf = IMG_Load(path);
    if (!surf)
    {
        printf("Error IMG_Load() failure %s\n", IMG_GetError());
        return NULL;
    }
    g->screen.clr_textures[g->screen.texture_count].texture = SDL_CreateTextureFromSurface(a->renderer, surf);
    if (!g->screen.clr_textures[g->screen.texture_count].texture)
    {
        printf("Error SDL_CreateTextureFromSurface() failure %s\n", SDL_GetError());
        return NULL;
    }
    SDL_FreeSurface(surf);

    ++g->screen.texture_count;

    return &g->screen.clr_textures[g->screen.texture_count - 1];
}
struct visual *game_add_visual(struct game *g, int x, int y, int w, int h, struct colored_texture *tex,
                               enum object_response_trigger_types rf, void (*func)(void *game, void *app, void *pRef), void *func_pr)
{
    if (g->screen.object_count >= OBJECT_CAP)
    {
        printf("Error too many objects\n");
        return NULL;
    }
    g->screen.objects[g->screen.object_count].rect.x = x;
    g->screen.objects[g->screen.object_count].rect.y = y;
    g->screen.objects[g->screen.object_count].rect.w = w;
    g->screen.objects[g->screen.object_count].rect.h = h;
    g->screen.objects[g->screen.object_count].pTexture = tex;
    g->screen.objects[g->screen.object_count].response_trigger = rf;
    g->screen.objects[g->screen.object_count].func = func;

    if (func_pr != NULL)
        g->screen.objects[g->screen.object_count].func_pRef = func_pr;
    if (func_pr == NULL)
        g->screen.objects[g->screen.object_count].func_pRef = &g->screen.objects[g->screen.object_count];

    SDL_Color clr;
    clr.r = 255;
    clr.g = 255;
    clr.b = 255;
    clr.a = 255;
    g->screen.objects[g->screen.object_count].color = clr;
    ++g->screen.object_count;

    return &g->screen.objects[g->screen.object_count - 1];
}
void game_destroy_visual(struct game *g, struct visual *v)
{
    bool found = false;
    struct visual *plast_obj = &g->screen.objects[g->screen.object_count - 1];
    for (int i = 0; i < g->screen.object_count; ++i)
    {
        if (v == &g->screen.objects[i])
        {
            found = true;
            printf("wow %i vs %i\n", v, &g->screen.objects[i]);
            g->screen.objects[i] = *plast_obj;
        }
    }
    if (!found)
    {
        return;
    }

    visual_destroy(plast_obj);
    --g->screen.object_count;
}
void game_change_text(struct game *g, struct application *a, struct text *t, char updated_text[TEXT_BUFFER_CAP])
{
    const char *t1 = t->text;
    const char *t2 = updated_text;
    if (strcmp(t1, t2) == 0)
        return;
    strcpy(t->text, updated_text);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(g->screen.font, t->text, white);
    if (!surfaceMessage)
    {
        printf("Error TTF_RenderText_Solid() failure\n");
        return;
    }

    t->texture = SDL_CreateTextureFromSurface(a->renderer, surfaceMessage);
    if (!t->texture)
    {
        printf("Error SDL_CreateTextureFromSurface() failure\n");
        return;
    }

    TTF_SizeText(g->screen.font, t->text, &t->graphic.rect.w, &t->graphic.rect.h);
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
struct text *game_add_text(struct game *g, struct application *a, int x, int y, char *t,
                           enum object_response_trigger_types rf, void (*func)(void *game, void *app, void *pRef), void *func_pr)
{
    if (g->screen.font == NULL)
    {
        printf("Error called game_add_text() with no font loaded\n");
        return NULL;
    }
    if (g->screen.text_object_count >= TEXT_OBJECT_CAP)
    {
        printf("Error too much text\n");
        return NULL;
    }
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(g->screen.font, t, white);
    if (!surfaceMessage)
    {
        printf("Error TTF_RenderText_Solid() failure\n");
        return NULL;
    }

    g->screen.text_objects[g->screen.text_object_count].texture = SDL_CreateTextureFromSurface(a->renderer, surfaceMessage);
    if (!g->screen.text_objects[g->screen.text_object_count].texture)
    {
        printf("Error SDL_CreateTextureFromSurface() failure\n");
        return NULL;
    }

    g->screen.text_objects[g->screen.text_object_count].graphic.rect.x = x;
    g->screen.text_objects[g->screen.text_object_count].graphic.rect.y = y;
    TTF_SizeText(g->screen.font, t, &g->screen.text_objects[g->screen.text_object_count].graphic.rect.w, &g->screen.text_objects[g->screen.text_object_count].graphic.rect.h);
    strcpy(g->screen.text_objects[g->screen.text_object_count].text, t);

    g->screen.text_objects[g->screen.text_object_count].graphic.rect.x = x;
    g->screen.text_objects[g->screen.text_object_count].graphic.rect.y = y;
    g->screen.text_objects[g->screen.text_object_count].graphic.response_trigger = rf;
    g->screen.text_objects[g->screen.text_object_count].graphic.func = func;
    if (func_pr != NULL)
        g->screen.text_objects[g->screen.text_object_count].graphic.func_pRef = func_pr;
    if (func_pr == NULL)
        g->screen.text_objects[g->screen.text_object_count].graphic.func_pRef = &g->screen.text_objects[g->screen.text_object_count];
    ;
    ++g->screen.text_object_count;

    return &g->screen.text_objects[g->screen.text_object_count - 1];
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
Mix_Chunk *game_add_sound(struct game *g, const char *path)
{
    if (g->audio.sound_count >= SOUND_CAP)
    {
        printf("Error too many sounds\n");
        return NULL;
    }
    g->audio.sounds[g->audio.sound_count] = Mix_LoadWAV(path);
    if (!g->audio.sounds[g->audio.sound_count])
    {
        printf("Error Mix_LoadWAV() failure %s\n", Mix_GetError());
        return NULL;
    }
    ++g->audio.sound_count;

    return g->audio.sounds[g->audio.sound_count - 1];
}
void game_set_sound_volume(struct game *g, Mix_Chunk *s, int volume)
{
    s->volume = volume;
}
void game_play_sound(struct game *g, Mix_Chunk *s)
{
    Mix_PlayChannel(-1, s, 0);
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
        if (g->screen.objects[i].pTexture == NULL)
            continue;

        if (colors_different(g->screen.objects[i].color, g->screen.objects[i].pTexture->color))
        {
            SDL_Color clr1 = g->screen.objects[i].color;
            SDL_Color clr2 = g->screen.objects[i].pTexture->color;
            texture_set_color(g->screen.objects[i].pTexture->texture, g->screen.objects[i].color);
            g->screen.objects[i].pTexture->color = g->screen.objects[i].color;
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
        SDL_RenderCopy(a->renderer, g->screen.objects[i].pTexture->texture, NULL, &temp_rect);
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
        visual_destroy(&g->screen.objects[i]);
    }
    g->screen.object_count = 0;
    for (int i = 0; i < g->screen.texture_count; ++i)
    {
        if (g->screen.clr_textures[i].texture != NULL)
        {
            SDL_DestroyTexture(g->screen.clr_textures[i].texture);
            g->screen.clr_textures[i].texture = NULL;
        }
        SDL_Color white;
        white.r = 255;
        white.g = 255;
        white.b = 255;
        white.a = 255;
        g->screen.clr_textures[i].color = white;
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

        visual_destroy(&g->screen.text_objects[i].graphic);
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

    for (int i = 0; i < g->timer_count; ++i)
    {
        g->timers[i].end_time = 0.0;
        g->timers[i].start_time = 0.0;
        g->timers[i].current_time = 0.0;
        g->timers[i].running = false;
        g->timers[i].speed = 0.0;
        g->timers[i].func = NULL;
        g->timers[i].start_func = NULL;
        g->timers[i].func_pRef = NULL;
        g->timers[i].start_func_pRef = NULL;
        g->timers[i].pLink = NULL;
        g->timers[i].pLinkInt8 = NULL;
        g->timers[i].restart_on_end = false;
        g->timers[i].timer_direction = 0;
    }
    g->timer_count = 0;

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

void handle_visual_responses(struct game *g, struct application *a, struct visual *v)
{
    visual_update_special(v);
    visual_set_special(v, 0, 0, 0, 0);

    switch (v->response_trigger)
    {
    case OBJ_RESPONSE_CLICKED:
        if (!mouse_collision(g, v))
            break;

        visual_set_special_size(v, v->rect.w * 0.25, v->rect.h * 0.25);
        if (g->mousePressed)
        {
            visual_set_special_size(v, v->rect.w * -0.25, v->rect.h * -0.25);
        }
        if (g->mouseClicked)
        {
            visual_set_special_size(v, v->rect.w * 2, v->rect.h * 2);
            if (v->func != NULL)
            {
                v->func(g, a, v->func_pRef);
            }
        }
        break;
    }
}
void logic_loop(struct game *g, struct application *a)
{
    // objects
    for (int i = 0; i < g->screen.object_count; ++i)
    {
        handle_visual_responses(g, a, &g->screen.objects[i]);
    }

    // text (please combine this with objects somehow)
    for (int i = 0; i < g->screen.text_object_count; ++i)
    {
        handle_visual_responses(g, a, &g->screen.text_objects[i].graphic);
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