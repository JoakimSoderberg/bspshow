
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "bsp.h"
#include "bsphelper.h"

typedef struct config_s
{
	float cx, cy, cz;
	float x, y, z;
	float direction;
	float updown;
	int window_width;
	int window_height;
	int left_mouse;
	int right_mouse;
	int down_mouse_x;
	int down_mouse_y;
	float clip_height;
	float min_edge_length;
	float min_area;

	int perspective;
	float zoom;
} config_t;

extern config_t config;

#endif // __CONFIG_H__
