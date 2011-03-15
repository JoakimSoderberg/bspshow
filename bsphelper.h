
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
	texture_t	*texture;
	dvertex_t	*vertices;
	float		*normal;

	dface_t		*face;
	dedge_t		*edges;
	dplane_t	*plane;
} polygon_t;

float calculate_face_area(dface_t *face);
int read_textures();
void upload_texture(texture_t *t, const miptex_t *mip, const byte *texture_data, int mip_level);

#endif // __BSPHELPER_H__
