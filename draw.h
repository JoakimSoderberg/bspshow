
#ifndef __DRAW_H__
#define __DRAW_H__

#include "bspfile.h"

void draw_text(float x, float y, float z, const char *format, ...);
void draw_bounding_box(dmodel_t *m);
void draw_axis();
void draw_face(dface_t *face);
void place_camera();

#endif // __DRAW_H__


