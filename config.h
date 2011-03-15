
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

extern dface_t *faces;
extern size_t face_count;

extern dedge_t *edges;
extern size_t edge_count;

extern int *ledges;
extern size_t ledge_count;

extern dvertex_t *vertices;
extern size_t vertex_count;

extern dmodel_t *models;
extern size_t model_count;

extern dplane_t *planes;
extern size_t plane_count;

extern texture_t *textures;
extern size_t texture_count;

// Texture related.
extern dmiptexlump_t *mip_header;
extern miptex_t **mipmap_ptrs;
extern size_t mipmap_count;

extern texinfo_t *textureinfos;
extern size_t texinfo_count;

#endif // __CONFIG_H__
