#ifndef DATA_H
#define DATA_H

#include "functions.h"

void level_default_loop(struct game *g, struct application *a);

struct level_default_data_struct
{
    // level data goes here
};
struct level_default_data_struct level_default_data;

void level_default(struct game *g, struct application *a)
{
    // level initiation goes here

    game_set_level_loop_func(g, level_default_loop);
}
void level_default_loop(struct game *g, struct application *a)
{
    // level loop content goes here
}

#endif