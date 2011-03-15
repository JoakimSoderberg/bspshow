
#ifndef __BSPHELPER_H__
#define __BSPHELPER_H__
#include <stdarg.h>
#include "bspfile.h"

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

#endif // __BSPHELPER_H__
