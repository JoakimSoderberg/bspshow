
#ifndef __BSPHELPER_H__
#define __BSPHELPER_H__

#include <stdarg.h>
#include "bsp.h"

typedef struct texture_s
{
	char	name[16];
	int		width;
	int		height;
	float	s[3];
	float	distS;
	float	t[3];
	float	distT;
	unsigned int texnum;
	int		is_loaded;
} texture_t;

typedef struct polygon_s
{
	dvertex_t	**vertices;
	dedge_t		**edges;
	size_t		vertex_count;
	size_t		edge_count;

	dface_t		*face;
	dplane_t	*plane;
	texture_t	*texture;
} polygon_t;

typedef struct bsp_helper_s
{
	bsp_t *bsp;
	
	polygon_t *polygons;
	size_t polygon_count;

} bsp_helper_t;

polygon_t *bsp_build_polygon_list(bsp_t *bsp, size_t *count);
float calculate_face_area(dface_t *face);
int is_edge_long_enough(dvertex_t *v0, dvertex_t *v1);
int read_textures();
void upload_texture(texture_t *t, const miptex_t *mip, const byte *texture_data, int mip_level);

#endif // __BSPHELPER_H__
