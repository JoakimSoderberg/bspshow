#ifndef __UTILS_H__
#define __UTILS_H__

#include "bspfile.h"

#ifdef _WIN32

int snprintf(char *buffer, size_t count, char const *format, ...);

int vsnprintf(char *buffer, size_t count, const char *format, va_list argptr);

#endif

#if defined(__linux__) || defined(_WIN32)

size_t strlcpy(char *dst, const char *src, size_t siz);

size_t strlcat(char *dst, const char *src, size_t siz);

#endif

#define DotProduct(x,y)			((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2])
#define VectorSubtract(a,b,c)	((c)[0] = (a)[0] - (b)[0], (c)[1] = (a)[1] - (b)[1], (c)[2] = (a)[2] - (b)[2])
#define VectorAdd(a,b,c)		((c)[0] = (a)[0] + (b)[0], (c)[1] = (a)[1] + (b)[1], (c)[2] = (a)[2] + (b)[2])
#define VectorCopy(a,b)			((b)[0] = (a)[0], (b)[1] = (a)[1], (b)[2] = (a)[2])
#define VectorClear(a)			((a)[0] = (a)[1] = (a)[2] = 0)
#define VectorNegate(a,b)		((b)[0] = -(a)[0], (b)[1] = -(a)[1], (b)[2] = -(a)[2])
#define VectorSet(v, x, y, z)	((v)[0] = ((x)), (v)[1] = (y), (v)[2] = (z))
#define CrossProduct(v1, v2, x)							\
	((x)[0] = (v1)[1] * (v2)[2] - (v1)[2] * (v2)[1],	\
	(x)[1] = (v1)[2] * (v2)[0] - (v1)[0] * (v2)[2],		\
	(x)[2] = (v1)[0] * (v2)[1] - (v1)[1] * (v2)[0])

void vector_copy(float v0[3], float v1[3]);

float vector_length(float v[3]);

float calculate_distance(dvertex_t *v0, dvertex_t *v1);

#endif // __UTILS_H__
