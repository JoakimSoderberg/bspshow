
#ifndef __BSP_H__
#define __BSP_H__

#include "bspfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
//typedef unsigned char byte;
/*
int open_bsp(const char *filename);
dvertex_t *get_vertices(size_t *count);
dface_t *get_faces(size_t *count);
dedge_t *get_edges(size_t *count);
int *get_ledges(size_t *count);
dmodel_t *get_models(size_t *count);
dplane_t *get_planes(size_t *count);
*/
texinfo_t *get_texinfos(size_t *count);
byte *get_miptexture_lump_data();
miptex_t **get_miptextures(const byte *texture_lump_data, size_t *count);
byte *get_texture(const byte *texture_lump_data, const miptex_t *miptex, int mip_level);

// ---------

typedef struct bsp_s
{
	dheader_t header;

	dface_t *faces;
	size_t face_count;

	dedge_t *edges;
	size_t edge_count;

	int *ledges;
	size_t ledge_count;

	dvertex_t *vertices;
	size_t vertex_count;

	dmodel_t *models;
	size_t model_count;

	dplane_t *planes;
	size_t plane_count;

	/*
	texture_t *textures;
	size_t texture_count;
*/
	// Texture related.
	byte *mip_data;
	miptex_t **mip_list;
	size_t mipmap_count;

	texinfo_t *texinfos;
	size_t texinfo_count;
	
} bsp_t;

int bsp_read_file(const char *filename, bsp_t *bsp);

int bsp_read_header(FILE *f, dheader_t *header);

#endif // __BSP_H__
