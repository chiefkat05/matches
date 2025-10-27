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
#define SIMULTANIOUS_SOUND_CAP 12
#define TEXT_BUFFER_CAP 100
#define TEXT_OBJECT_CAP 14

int random_int(int min, int max)
{
    return (int)((rand() / (RAND_MAX * 1.0f)) * (max - min) + min);
}

// set these to real bit values??
enum object_response_flags
{
    OBJ_RESPONSE_CLICKED = 1,
    OBJ_RESPONSE_DRAGGED = 2
};

struct visual
{
    SDL_Rect rect;
    int sx, sy;
    int sw, sh;

    int real_sx, real_sy;
    int real_sw, real_sh;

    unsigned int texture;
    enum object_response_flags response_flags;
    void (*func)(void *game, void *app, unsigned int i);
};
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
    struct graphic_layer screen;
    struct audio_bank audio;
    bool running;
    int mouseX, mouseY;
    bool mousePressed, mouseClicked, mouseReleased;
    double game_time;
    int score;
    double window_x_scale;
    double window_y_scale;
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

void game_add_texture(struct game *g, struct application *a, const char *path)
{
    if (g->screen.texture_count >= TEXTURE_CAP)
    {
        printf("Error too many textures\n");
        return;
    }
    SDL_Surface *surf = IMG_Load(path);
    if (!surf)
    {
        printf("Error IMG_Load() failure %s\n", IMG_GetError());
        return;
    }
    g->screen.textures[g->screen.texture_count] = SDL_CreateTextureFromSurface(a->renderer, surf);
    if (!g->screen.textures[g->screen.texture_count])
    {
        printf("Error SDL_CreateTextureFromSurface() failure %s\n", SDL_GetError());
        return;
    }
    SDL_FreeSurface(surf);

    ++g->screen.texture_count;
}
void game_add_visual(struct game *g, int x, int y, int w, int h, unsigned int t,
                     enum object_response_flags rf, void (*func)(void *game, void *app, unsigned int i))
{
    if (g->screen.object_count >= OBJECT_CAP)
    {
        printf("Error too many objects\n");
        return;
    }
    if (t > g->screen.texture_count)
    {
        printf("Error requested texture %i does not exist\n", t);
        return;
    }
    g->screen.objects[g->screen.object_count].rect.x = x;
    g->screen.objects[g->screen.object_count].rect.y = y;
    g->screen.objects[g->screen.object_count].rect.w = w;
    g->screen.objects[g->screen.object_count].rect.h = h;
    g->screen.objects[g->screen.object_count].texture = t;
    g->screen.objects[g->screen.object_count].response_flags = rf;
    g->screen.objects[g->screen.object_count].func = func;
    ++g->screen.object_count;
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
void game_add_text(struct game *g, struct application *a, int x, int y, char *t,
                   enum object_response_flags rf, void (*func)(void *game, void *app, unsigned int i))
{
    if (g->screen.text_object_count >= TEXT_OBJECT_CAP)
    {
        printf("Error too much text\n");
        return;
    }
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(g->screen.font, t, white);
    if (!surfaceMessage)
    {
        printf("Error TTF_RenderText_Solid() failure\n");
        return;
    }

    g->screen.text_objects[g->screen.text_object_count].texture = SDL_CreateTextureFromSurface(a->renderer, surfaceMessage);
    if (!g->screen.text_objects[g->screen.text_object_count].texture)
    {
        printf("Error SDL_CreateTextureFromSurface() failure\n");
        return;
    }

    g->screen.text_objects[g->screen.text_object_count].graphic.rect.x = x;
    g->screen.text_objects[g->screen.text_object_count].graphic.rect.y = y;
    TTF_SizeText(g->screen.font, t, &g->screen.text_objects[g->screen.text_object_count].graphic.rect.w, &g->screen.text_objects[g->screen.text_object_count].graphic.rect.h);
    strcpy(g->screen.text_objects[g->screen.text_object_count].text, t);

    g->screen.text_objects[g->screen.text_object_count].graphic.rect.x = x;
    g->screen.text_objects[g->screen.text_object_count].graphic.rect.y = y;
    g->screen.text_objects[g->screen.text_object_count].graphic.response_flags = rf;
    g->screen.text_objects[g->screen.text_object_count].graphic.func = func;
    ++g->screen.text_object_count;
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
void game_add_sound(struct game *g, const char *path)
{
    if (g->audio.sound_count >= SOUND_CAP)
    {
        printf("Error too many sounds\n");
        return;
    }
    g->audio.sounds[g->audio.sound_count] = Mix_LoadWAV(path);
    if (!g->audio.sounds[g->audio.sound_count])
    {
        printf("Error Mix_LoadWAV() failure %s\n", Mix_GetError());
        return;
    }
    ++g->audio.sound_count;
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

void game_draw(struct game *g, struct application *a)
{
    for (int i = 0; i < g->screen.object_count; ++i)
    {
        if (g->screen.objects[i].texture < 0)
            continue;

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

// game functions
void newRandomPosition(struct game *g, unsigned int index)
{
    int rand_gen_sound = random_int(0, 5);
    int rand_gen_volume = random_int(30, 60);
    game_set_sound_volume(g, rand_gen_sound, rand_gen_volume);
    game_play_sound(g, rand_gen_sound);
    int rand_gen_x = random_int(0, WINDOW_WIDTH - 100);
    int rand_gen_y = random_int(0, WINDOW_HEIGHT - 200);
    g->screen.objects[index].rect.x = rand_gen_x;
    g->screen.objects[index].rect.y = rand_gen_y;
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
void game_init(struct game *g, struct application *a)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("Error SDL_Init() failure\n");
        return;
    }

    a->window = SDL_CreateWindow("Shooter 01", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
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
    Mix_AllocateChannels(SIMULTANIOUS_SOUND_CAP + 1);

    game_set_music(g, "./audio/music/dull.ogg");
    game_add_sound(g, "./audio/sfx/collect_1.wav");
    game_add_sound(g, "./audio/sfx/collect_2.wav");
    game_add_sound(g, "./audio/sfx/collect_3.wav");
    game_add_sound(g, "./audio/sfx/collect_4.wav");
    game_add_sound(g, "./audio/sfx/collect_5.wav");
    game_play_music(g);
    game_play_sound(g, 0);

    if (TTF_Init() < 0)
    {
        printf("Error TTF_Init() failure %s\n", TTF_GetError());
        return;
    }

    g->screen.font = TTF_OpenFont("./scabber-font/Scabber-q2Mn0.ttf", 32);
    if (!g->screen.font)
    {
        printf("Error TTF_OpenFont failure() %s\n", TTF_GetError());
        return;
    }

    game_add_texture(g, a, "./graphics/game/idfk.png");
    game_add_texture(g, a, "./graphics/game/bg-1.png");
    game_add_visual(g, 0, 0, 1280, 720, 1, 0, NULL);
    game_add_visual(g, 100, 100, 10, 10, 0, OBJ_RESPONSE_CLICKED, updateScore10);
    game_add_visual(g, 250, 250, 50, 50, 0, OBJ_RESPONSE_CLICKED, updateScore3);
    game_add_visual(g, 500, 500, 100, 100, 0, OBJ_RESPONSE_CLICKED, updateScore1);

    game_add_text(g, a, 10, WINDOW_HEIGHT - 40, "Score: 0", 0, NULL);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 80, "Fullscreen", OBJ_RESPONSE_CLICKED, set_fullscreen);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 120, "Windowed", OBJ_RESPONSE_CLICKED, set_windowed);
    game_add_text(g, a, WINDOW_WIDTH - 240, WINDOW_HEIGHT - 40, "Quit", OBJ_RESPONSE_CLICKED, set_quit);

    g->running = true;
    g->game_time = 0.0;
    g->score = 0;
}

void game_exit(struct game *g, struct application *a)
{
    TTF_CloseFont(g->screen.font);
    for (int i = 0; i < g->screen.texture_count; ++i)
    {
        SDL_DestroyTexture(g->screen.textures[i]);
    }
    SDL_DestroyRenderer(a->renderer);
    SDL_DestroyWindow(a->window);
}

void draw_loop(struct game *g, struct application *a);
void event_loop(struct game *g);
void logic_loop(struct game *g, struct application *a);

bool game_loop(struct game *g, struct application *a)
{
    g->game_time += 0.01;
    SDL_SetRenderDrawColor(a->renderer, 90, 90, 200, 255);
    SDL_RenderClear(a->renderer);

    event_loop(g);
    logic_loop(g, a);

    char scoretext[TEXT_BUFFER_CAP] = "Score: ";
    char input[sizeof(long)];
    sprintf(input, "%d", g->score);
    strcat(scoretext, input);
    game_change_text(g, a, 0, scoretext);

    draw_loop(g, a);

    SDL_Delay(1000 / 60);

    if (g->running == false)
        return false;

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
                        g->screen.objects[i].func(g, a, i);
                    }
                }
            }
        }
    }

    // text
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
                        g->screen.text_objects[i].graphic.func(g, a, i);
                    }
                }
            }
        }
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