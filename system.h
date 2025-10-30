#ifndef SYSTEM_H
#define SYSTEM_H

#include "definitions.h"

void v_animation_init(struct visual_animation *va, struct visual *v)
{
    va->pVisual = v;
    va->frame_count = 0;
    va->time = 0.0;
    va->current_frame = 0;
    va->running = false;
}
void v_animation_add_frame(struct visual_animation *va, double et, unsigned int pos_i, void (*func)(void *g, void *a, void *pr), void *func_pr)
{
    if (va->frame_count >= VISUAL_ANIMATION_FRAME_LIMIT)
    {
        printf("Error too many frames in animation\n");
        return;
    }

    va->frames[va->frame_count] = malloc(sizeof(struct visual_animation_frame));
    va->frames[va->frame_count]->end_time = et;
    va->frames[va->frame_count]->pos_index = pos_i;
    va->frames[va->frame_count]->func = func;

    va->frames[va->frame_count]->func_pRef = NULL;
    if (func_pr != NULL)
        va->frames[va->frame_count]->func_pRef = func_pr;
    if (func_pr == NULL && va->pVisual != NULL)
        va->frames[va->frame_count]->func_pRef = va->pVisual;

    ++va->frame_count;
}

void v_animation_update(struct visual_animation *va, void *g, void *a)
{
    if (!va->running)
        return;

    va->time += VISUAL_ANIMATION_SPEED;
    // frame end
    if (va->time >= va->frames[va->current_frame]->end_time)
    {
        va->time = 0.0;
        ++va->current_frame;
        // animation end
        if (va->current_frame >= va->frame_count)
        {
            va->current_frame = 0;
            va->running = false;
        }

        if (va->frames[va->current_frame]->func != NULL)
        {
            va->frames[va->current_frame]->func(g, a, va->frames[va->current_frame]->func_pRef);
        }
        va->pVisual->animation_rect.x = va->pVisual->animation_rect.w * (va->frames[va->current_frame]->pos_index % va->pVisual->x_frames);
        va->pVisual->animation_rect.y = va->pVisual->animation_rect.h * (va->frames[va->current_frame]->pos_index / va->pVisual->x_frames);
    }
}
void v_animation_play(struct visual_animation *va)
{
    va->running = true;
}

bool input_held(struct input_action_data *data)
{
    return data->held;
}
bool input_just_pressed(struct input_action_data *data)
{
    return data->just_pressed;
}
bool input_just_released(struct input_action_data *data)
{
    return data->just_released;
}

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
void timer_unlink(struct timer *t)
{
    t->pLink = NULL;
    t->pLinkInt8 = NULL;
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

void visual_copy(struct visual *src, struct visual *dest)
{
    dest->rect = src->rect;
    dest->color = src->color;
    dest->sx = src->sx;
    dest->sy = src->sy;
    dest->sw = src->sw;
    dest->sh = src->sh;
    dest->real_sx = src->real_sx;
    dest->real_sy = src->real_sy;
    dest->real_sw = src->real_sw;
    dest->real_sh = src->real_sh;

    dest->func = src->func;
    dest->func_pRef = src->func_pRef;
    dest->pTexture = src->pTexture;
    dest->object_type_flags = src->object_type_flags;
}
void visual_set_interaction(struct visual *v, int rt, void (*func)(void *g, void *a, void *pr), void *func_pr)
{
    v->object_type_flags = rt;
    v->func = func;
    v->func_pRef = func_pr;
}
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
    v->object_type_flags = 0;
    v->sx = 0.0;
    v->sy = 0.0;
    v->sw = 0.0;
    v->sh = 0.0;
    v->real_sx = 0.0;
    v->real_sy = 0.0;
    v->real_sw = 0.0;
    v->real_sh = 0.0;
    if (strcmp(v->text_str, "") != 0)
    {
        strcpy(v->text_str, "");
        SDL_DestroyTexture(v->pTexture->texture);
        v->pTexture->texture = NULL;
    }
    v->pTexture = NULL;
}

void game_init_input(struct game *g)
{
    for (int i = 0; i < input_action_type_limit; ++i)
    {
        g->input_actions[i].held = false;
        g->input_actions[i].just_pressed = false;
        g->input_actions[i].just_released = false;
        g->input_actions[i].type = (enum input_action_type)i;
        for (int j = 0; j < INPUT_VALID_SCANCODE_LIMIT; ++j)
        {
            g->input_actions[i].valid_scancodes[j] = 0;
        }
    }
    g->input_action_being_changed = INPUT_NONE;
}
void game_add_input_key(struct game *g, enum input_action_type action, uint8_t key)
{
    if (g->input_actions[action].scancode_count >= INPUT_VALID_SCANCODE_LIMIT)
    {
        printf("Error too many keycodes on input action %i\n", action);
        return;
    }

    g->input_actions[action].valid_scancodes[g->input_actions[action].scancode_count] = key;
    ++g->input_actions[action].scancode_count;
}
void game_reset_input_keys(struct game *g, enum input_action_type action)
{
    for (int i = 0; i < g->input_actions[action].scancode_count; ++i)
    {
        g->input_actions[action].valid_scancodes[i] = 0;
    }
    g->input_actions[action].scancode_count = 0;
}
void game_update_input(struct game *g)
{
    const uint8_t *keyboard_state = SDL_GetKeyboardState(NULL);

    for (int i = 0; i < input_action_type_limit; ++i)
    {
        bool anypressed = false;

        g->input_actions[i].just_pressed = false;
        g->input_actions[i].just_released = false;
        for (int j = 0; j < INPUT_VALID_SCANCODE_LIMIT; ++j)
        {
            if (keyboard_state[g->input_actions[i].valid_scancodes[j]])
                anypressed = true;
        }

        if (!anypressed)
        {
            if (g->input_actions[i].held)
                g->input_actions[i].just_released = true;

            g->input_actions[i].held = false;
        }
        if (anypressed)
        {
            if (!g->input_actions[i].held)
                g->input_actions[i].just_pressed = true;

            g->input_actions[i].held = true;
        }
    }
}
void game_update_input_change(struct game *g)
{
    if (g->input_action_being_changed == INPUT_NONE)
        return;

    int keycount;
    const uint8_t *map = SDL_GetKeyboardState(&keycount);

    for (int i = 0; i < keycount; ++i)
    {
        if (i == SDL_SCANCODE_ESCAPE && map[i])
        {
            game_reset_input_keys(g, g->input_action_being_changed);
            g->input_action_just_changed = g->input_action_being_changed;
            g->input_action_being_changed = INPUT_NONE;
            return;
        }
        if (map[i])
        {
            game_add_input_key(g, g->input_action_being_changed, i);
            g->input_action_just_changed = g->input_action_being_changed;
            g->input_action_being_changed = INPUT_NONE;
            return;
        }
    }
}
void game_do_on_input_change(struct game *g, struct application *a, void *func_pr, enum input_action_type action, void (*func)(void *g, void *a, void *pr))
{
    if (g->input_action_just_changed != action)
        return;

    if (func != NULL)
    {
        func(g, a, func_pr);
    }
}
void game_change_input(struct game *g, enum input_action_type action)
{
    g->input_action_being_changed = action;
}
bool game_input_just_pressed(struct game *g, enum input_action_type action)
{
    return input_just_pressed(&g->input_actions[action]);
}
bool game_input_held(struct game *g, enum input_action_type action)
{
    return input_held(&g->input_actions[action]);
}
bool game_input_just_released(struct game *g, enum input_action_type action)
{
    return input_just_released(&g->input_actions[action]);
}
const char *game_get_key_name(struct game *g, enum input_action_type action, unsigned int key_index)
{
    SDL_Keycode key = SDL_GetKeyFromScancode(g->input_actions[action].valid_scancodes[key_index]);
    return SDL_GetKeyName(key);
}
char *game_get_action_key_names_list(struct game *g, enum input_action_type action)
{
    char key_name_list[TEXT_BUFFER_CAP] = "";
    for (int i = 0; i < g->input_actions[action].scancode_count; ++i)
    {
        const char *key_name = game_get_key_name(g, action, i);
        strcat(key_name_list, key_name);

        if (i < g->input_actions[action].scancode_count - 1)
            strcat(key_name_list, ", ");
    }
    char *output = malloc(sizeof(char) * TEXT_BUFFER_CAP);
    strcpy(output, key_name_list);

    return output;
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
void game_timer_set_func(struct game *g, struct timer *t, void (*func)(void *g, void *a, void *pr), void *func_pr)
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
void game_timer_set_start_func(struct game *g, struct timer *t, void (*func)(void *g, void *a, void *pr), void *func_pr)
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
struct visual *game_add_visual(struct game *g, int x, int y, int w, int h, int z, struct colored_texture *tex,
                               int rf, void (*func)(void *game, void *app, void *pRef), void *func_pr)
{
    if (g->screen.object_count >= OBJECT_CAP)
    {
        printf("Error too many objects\n");
        return NULL;
    }

    g->screen.objects[g->screen.object_count] = malloc(sizeof(struct visual)); // bad idea??
    g->screen.objects[g->screen.object_count]->rect.x = x;
    g->screen.objects[g->screen.object_count]->rect.y = y;
    g->screen.objects[g->screen.object_count]->rect.w = w;
    g->screen.objects[g->screen.object_count]->rect.h = h;
    g->screen.objects[g->screen.object_count]->z_index = z;
    g->screen.objects[g->screen.object_count]->sx = 0;
    g->screen.objects[g->screen.object_count]->sy = 0;
    g->screen.objects[g->screen.object_count]->sw = 0;
    g->screen.objects[g->screen.object_count]->sh = 0;
    g->screen.objects[g->screen.object_count]->real_sx = 0;
    g->screen.objects[g->screen.object_count]->real_sy = 0;
    g->screen.objects[g->screen.object_count]->real_sw = 0;
    g->screen.objects[g->screen.object_count]->real_sh = 0;
    g->screen.objects[g->screen.object_count]->pTexture = tex;
    g->screen.objects[g->screen.object_count]->object_type_flags = rf;
    g->screen.objects[g->screen.object_count]->func = func;
    g->screen.objects[g->screen.object_count]->text_str = malloc(sizeof(char));
    strcpy(g->screen.objects[g->screen.object_count]->text_str, "");
    g->screen.objects[g->screen.object_count]->x_frames = 1;
    g->screen.objects[g->screen.object_count]->y_frames = 1;
    g->screen.objects[g->screen.object_count]->animation_rect.x = 0;
    g->screen.objects[g->screen.object_count]->animation_rect.y = 0;
    SDL_QueryTexture(g->screen.objects[g->screen.object_count]->pTexture->texture, NULL, NULL,
                     &g->screen.objects[g->screen.object_count]->animation_rect.w, &g->screen.objects[g->screen.object_count]->animation_rect.h);

    if (func_pr != NULL)
        g->screen.objects[g->screen.object_count]->func_pRef = func_pr;
    if (func_pr == NULL)
        g->screen.objects[g->screen.object_count]->func_pRef = g->screen.objects[g->screen.object_count];

    SDL_Color clr;
    clr.r = 255;
    clr.g = 255;
    clr.b = 255;
    clr.a = 255;
    g->screen.objects[g->screen.object_count]->color = clr;
    ++g->screen.object_count;
    g->need_z_reorder = true;

    return g->screen.objects[g->screen.object_count - 1];
}
void visual_set_frames(struct visual *v, unsigned int xf, unsigned int yf)
{
    v->x_frames = xf;
    v->y_frames = yf;

    SDL_QueryTexture(v->pTexture->texture, NULL, NULL,
                     &v->animation_rect.w, &v->animation_rect.h);
    v->animation_rect.w /= v->x_frames;
    v->animation_rect.h /= v->y_frames;
}
void game_destroy_visual(struct game *g, struct visual *v)
{
    bool found = false;
    struct visual *plast_obj = g->screen.objects[g->screen.object_count - 1];

    for (int i = 0; i < g->screen.object_count; ++i)
    {
        if (g->screen.objects[i] == v)
        {
            g->screen.objects[i] = plast_obj;
            found = true;
        }
    }

    if (!found)
        return;

    free(v); // also bad idea???
    --g->screen.object_count;
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
struct visual *game_add_text(struct game *g, struct application *a, int x, int y, int z, char *t,
                             int rf, void (*func)(void *game, void *app, void *pRef), void *func_pr)
{
    if (g->screen.font == NULL)
    {
        printf("Error called game_add_text() with no font loaded\n");
        return NULL;
    }
    if (g->screen.object_count >= OBJECT_CAP)
    {
        printf("Error too many objects\n");
        return NULL;
    }
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(g->screen.font, t, white);
    if (!surfaceMessage)
    {
        printf("Error TTF_RenderText_Solid() failure\n");
        return NULL;
    }

    g->screen.objects[g->screen.object_count] = malloc(sizeof(struct visual)); // bad idea??
    g->screen.objects[g->screen.object_count]->pTexture = malloc(sizeof(struct colored_texture));

    g->screen.objects[g->screen.object_count]->pTexture->texture = SDL_CreateTextureFromSurface(a->renderer, surfaceMessage);
    if (!g->screen.objects[g->screen.object_count]->pTexture->texture)
    {
        printf("Error SDL_CreateTextureFromSurface() failure\n");
        return NULL;
    }

    g->screen.objects[g->screen.object_count]->rect.x = x;
    g->screen.objects[g->screen.object_count]->rect.y = y;
    TTF_SizeText(g->screen.font, t, &g->screen.objects[g->screen.object_count]->rect.w, &g->screen.objects[g->screen.object_count]->rect.h);
    g->screen.objects[g->screen.object_count]->x_frames = 1;
    g->screen.objects[g->screen.object_count]->y_frames = 1;
    g->screen.objects[g->screen.object_count]->animation_rect.x = 0;
    g->screen.objects[g->screen.object_count]->animation_rect.y = 0;
    TTF_SizeText(g->screen.font, t, &g->screen.objects[g->screen.object_count]->animation_rect.w, &g->screen.objects[g->screen.object_count]->animation_rect.h);
    g->screen.objects[g->screen.object_count]->z_index = z;
    g->screen.objects[g->screen.object_count]->text_str = malloc(sizeof(char) * TEXT_BUFFER_CAP);
    strcpy(g->screen.objects[g->screen.object_count]->text_str, t);
    g->screen.objects[g->screen.object_count]->object_type_flags = rf;
    g->screen.objects[g->screen.object_count]->func = func;

    g->screen.objects[g->screen.object_count]->color = white;
    g->screen.objects[g->screen.object_count]->sx = 0.0;
    g->screen.objects[g->screen.object_count]->sy = 0.0;
    g->screen.objects[g->screen.object_count]->sw = 0.0;
    g->screen.objects[g->screen.object_count]->sh = 0.0;
    g->screen.objects[g->screen.object_count]->real_sx = 0.0;
    g->screen.objects[g->screen.object_count]->real_sy = 0.0;
    g->screen.objects[g->screen.object_count]->real_sw = 0.0;
    g->screen.objects[g->screen.object_count]->real_sh = 0.0;

    if (func_pr != NULL)
        g->screen.objects[g->screen.object_count]->func_pRef = func_pr;
    if (func_pr == NULL)
        g->screen.objects[g->screen.object_count]->func_pRef = &g->screen.objects[g->screen.object_count];
    ;
    ++g->screen.object_count;
    g->need_z_reorder = true;

    return g->screen.objects[g->screen.object_count - 1];
}
void game_change_text(struct game *g, struct application *a, struct visual *v, char *updated_text)
{
    if (strcmp(v->text_str, "") == 0)
    {
        printf("Error object at location %p is not a text_str object\n", v);
    }
    const char *t1 = v->text_str;
    const char *t2 = updated_text;
    if (strcmp(t1, t2) == 0)
        return;
    strcpy(v->text_str, updated_text);

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surfaceMessage = TTF_RenderText_Solid(g->screen.font, v->text_str, white);
    if (!surfaceMessage)
    {
        printf("Error TTF_RenderText_Solid() failure\n");
        return;
    }

    v->pTexture->texture = SDL_CreateTextureFromSurface(a->renderer, surfaceMessage);
    if (!v->pTexture->texture)
    {
        printf("Error SDL_CreateTextureFromSurface() failure\n");
        return;
    }
    v->animation_rect.x = 0;
    v->animation_rect.y = 0;
    TTF_SizeText(g->screen.font, v->text_str,
                 &v->animation_rect.w, &v->animation_rect.h);

    TTF_SizeText(g->screen.font, v->text_str, &v->rect.w, &v->rect.h);
    SDL_SetTextureColorMod(v->pTexture->texture, v->pTexture->color.r, v->pTexture->color.g, v->pTexture->color.b);
    SDL_SetTextureAlphaMod(v->pTexture->texture, v->pTexture->color.a);
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
        if (g->screen.objects[i]->pTexture == NULL)
            continue;

        if (colors_different(g->screen.objects[i]->color, g->screen.objects[i]->pTexture->color))
        {
            SDL_Color clr1 = g->screen.objects[i]->color;
            SDL_Color clr2 = g->screen.objects[i]->pTexture->color;
            texture_set_color(g->screen.objects[i]->pTexture->texture, g->screen.objects[i]->color);
            g->screen.objects[i]->pTexture->color = g->screen.objects[i]->color;
        }

        SDL_Rect temp_rect = g->screen.objects[i]->rect;
        temp_rect.x += g->screen.objects[i]->real_sx;
        temp_rect.y += g->screen.objects[i]->real_sy;
        temp_rect.w += g->screen.objects[i]->real_sw;
        temp_rect.h += g->screen.objects[i]->real_sh;

        temp_rect.x *= g->window_x_scale;
        temp_rect.w *= g->window_x_scale;
        temp_rect.y *= g->window_y_scale;
        temp_rect.h *= g->window_y_scale;

        SDL_RenderCopy(a->renderer, g->screen.objects[i]->pTexture->texture, &g->screen.objects[i]->animation_rect, &temp_rect);
    }
}

int comp_visual_z(const void *a, const void *b)
{
    struct visual **pObjA = (struct visual **)a;
    struct visual **pObjB = (struct visual **)b;
    return (*pObjA)->z_index - (*pObjB)->z_index;
}
void game_organize_draw_objects(struct game *g)
{
    if (!g->need_z_reorder)
        return;

    qsort(g->screen.objects, g->screen.object_count, sizeof(struct visual **), comp_visual_z);
    g->need_z_reorder = false;
}

void game_init(struct game *g, struct application *a)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        printf("Error SDL_Init() failure %s\n", SDL_GetError());
        return;
    }

    a->window = SDL_CreateWindow("Content", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
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

    game_init_input(g);

    g->level_initiated = false;
    g->running = true;
    g->need_z_reorder = true;
    game_organize_draw_objects(g);
    g->game_time = 0.0;
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
        visual_destroy(g->screen.objects[i]);
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
        game_organize_draw_objects(g);

        draw_loop(g, a);
    }

    SDL_Delay(1000 / 60);

    if (g->running == false)
    {
        return false;
    }

    return true;
}

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
bool visual_collision(struct visual *vA, struct visual *vB)
{
    double t;
    if ((t == vA->rect.x - vB->rect.x) > vB->rect.w || -t > vA->rect.w)
        return false;
    if ((t == vA->rect.y - vB->rect.y) > vB->rect.h || -t > vA->rect.h)
        return false;
    return true;
}

void handle_visual_responses(struct game *g, struct application *a, struct visual *v)
{
    visual_update_special(v);
    visual_set_special(v, 0, 0, 0, 0);

    if (OBJECT_TYPE_BUTTON & v->object_type_flags)
    {
        if (mouse_collision(g, v))
        {
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
        }
    }
    if (OBJECT_TYPE_GENERIC_2D_PLAYER & v->object_type_flags)
    {
        if (game_input_held(g, INPUT_DOWN))
        {
            v->rect.y += 10.0;
            if (v->func != NULL)
            {
                v->func(g, a, v->func_pRef);
            }
        }
        if (game_input_held(g, INPUT_RIGHT))
        {
            v->rect.x += 10.0;
            if (v->func != NULL)
            {
                v->func(g, a, v->func_pRef);
            }
        }
        if (game_input_held(g, INPUT_UP))
        {
            v->rect.y -= 10.0;
            if (v->func != NULL)
            {
                v->func(g, a, v->func_pRef);
            }
        }
        if (game_input_held(g, INPUT_LEFT))
        {
            v->rect.x -= 10.0;
            if (v->func != NULL)
            {
                v->func(g, a, v->func_pRef);
            }
        }
    }
    if (OBJECT_TYPE_COLLISION & v->object_type_flags)
    {
        if (v->func != NULL && visual_collision(v, (struct visual *)v->func_pRef))
        {
            v->func(g, a, v->func_pRef); // maybe not how this should be
        }
    }
}
void logic_loop(struct game *g, struct application *a)
{
    g->input_action_just_changed = INPUT_NONE;
    for (int i = 0; i < g->screen.object_count; ++i)
    {
        handle_visual_responses(g, a, g->screen.objects[i]);
    }

    for (int i = 0; i < g->timer_count; ++i)
    {
        timer_update(&g->timers[i], g, a);
    }

    game_update_input(g);
    game_update_input_change(g);
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

void load_file_data(const char *path, void *data, size_t data_unit_size, unsigned int data_location)
{
    FILE *inputFile;

    inputFile = fopen(path, "rb");
    if (!inputFile)
    {
        printf("Error failed to open file %s for input\n", path);
        return;
    }

    fseek(inputFile, data_location * data_unit_size, SEEK_SET);
    int success = fread(data, data_unit_size, 1, inputFile);
    if (!success)
    {
        printf("Error failed to read from file %s\n", path);
        return;
    }

    fclose(inputFile);
}
void save_file_data(const char *path, void *data, size_t data_unit_size, unsigned int data_count)
{
    FILE *outputFile;
    outputFile = fopen(path, "wb");
    if (!outputFile)
    {
        printf("Error failed to open file %s for output\n", path);
        return;
    }

    int success = fwrite(data, data_unit_size, data_count, outputFile);
    if (!success)
    {
        printf("Error writing to file %s\n", path);
        return;
    }

    fclose(outputFile);
}

#endif