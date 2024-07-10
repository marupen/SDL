#ifndef DRAW_H
#define DRAW_H

#include "stdafx.h"

//Rectangle clippingRect;
void DrawScene(SDL_Renderer *renderer, double angle_master, double angle_slave, int size_x, int size_y, int master_x1, int master_y1, int slave_x1, int slave_y1);

#endif