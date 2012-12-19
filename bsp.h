
#ifndef __BSP_H__
#define __BSP_H__

#include "bspfile.h"
#include <stdio.h>
#include <stdlib.h>
//typedef unsigned char byte;

int open_bsp(const char *filename);
dvertex_t *get_vertices(size_t *count);
dface_t *get_faces(size_t *count);
dedge_t *get_edges(size_t *count);
int *get_ledges(size_t *count);
dmodel_t *get_models(size_t *count);
dplane_t *get_planes(size_t *count);
texinfo_t *get_texinfos(size_t *count);
byte *get_miptexture_lump_data();
miptex_t **get_miptextures(const byte *texture_lump_data, size_t *count);
byte *get_texture(const byte *texture_lump_data, const miptex_t *miptex, int mip_level);

#endif // __BSP_H__
