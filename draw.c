
#include <math.h>
#ifdef WITH_FREEGLUT
#include <gl/freeglut.h>
#else
#include <glut.h>
#endif
#include <stdarg.h>
#include "bsp.h"
#include "draw.h"
#include "config.h"
#include "bsphelper.h"
#include "utils.h"

void draw_text(float x, float y, float z, const char *format, ...)
{
	static char buf[512];
	const char *c;
	va_list args;

	buf[0] = 0;
	
	glRasterPos3f(x, y,z);

	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);

	for (c = buf; *c != '\0'; c++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
	}
	
	va_end(args);
}

void draw_bounding_box(dmodel_t *m)
{ 
	int render_type = GL_LINE; // GL_FILL;
	
	glDisable(GL_CULL_FACE);

	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT, render_type);
	glPolygonMode(GL_BACK, render_type);
	
	glBegin(GL_POLYGON);
	{
		// Top.
		glColor4f(0.0, 1.0, 1.0, 0.25);
		glVertex3f(m->mins[0], m->mins[1], m->maxs[2]);

		glColor4f(1.0, 0.0, 0.0, 0.5);
		glVertex3f(m->mins[0], m->maxs[1], m->maxs[2]);

		glColor4f(0.0, 1.0, 0.0, 0.5);
		glVertex3f(m->maxs[0], m->maxs[1], m->maxs[2]);

		glColor4f(0.0, 0.0, 1.0, 0.5);
		glVertex3f(m->maxs[0], m->mins[1], m->maxs[2]);
	}
	glEnd();

	glBegin(GL_POLYGON);
	{
		// Bottom.
		glColor4f(0.0, 1.0, 1.0, 0.25);
		glVertex3f(m->mins[0], m->mins[1], m->mins[2]);

		glColor4f(1.0, 0.0, 0.0, 0.5);
		glVertex3f(m->mins[0], m->maxs[1], m->mins[2]);

		glColor4f(0.0, 1.0, 0.0, 0.5);
		glVertex3f(m->maxs[0], m->maxs[1], m->mins[2]);

		glColor4f(0.0, 0.0, 1.0, 0.5);
		glVertex3f(m->maxs[0], m->mins[1], m->mins[2]);
	}
	glEnd();

	glBegin(GL_POLYGON);
	{
		// Side 1.
		glColor4f(0.0, 1.0, 1.0, 0.25);
		glVertex3f(m->mins[0], m->mins[1], m->mins[2]);

		glColor4f(1.0, 0.0, 0.0, 0.5);
		glVertex3f(m->mins[0], m->mins[1], m->maxs[2]);

		glColor4f(0.0, 1.0, 0.0, 0.5);
		glVertex3f(m->mins[0], m->maxs[1], m->maxs[2]);

		glColor4f(0.0, 0.0, 1.0, 0.5);
		glVertex3f(m->mins[0], m->maxs[1], m->mins[2]);
	}
	glEnd();	

	glBegin(GL_POLYGON);
	{
		// Side 2.
		glColor4f(0.0, 1.0, 1.0, 0.25);
		glVertex3f(m->mins[0], m->mins[1], m->mins[2]);

		glColor4f(1.0, 0.0, 0.0, 0.5);
		glVertex3f(m->mins[0], m->mins[1], m->maxs[2]);

		glColor4f(0.0, 1.0, 0.0, 0.5);
		glVertex3f(m->maxs[0], m->mins[1], m->maxs[2]);

		glColor4f(0.0, 0.0, 1.0, 0.5);
		glVertex3f(m->maxs[0], m->mins[1], m->mins[2]);
	}
	glEnd();

	glBegin(GL_POLYGON);
	{
		// Side 3.
		glColor4f(0.0, 1.0, 1.0, 0.25);
		glVertex3f(m->maxs[0], m->mins[1], m->mins[2]);

		glColor4f(1.0, 0.0, 0.0, 0.5);
		glVertex3f(m->maxs[0], m->mins[1], m->maxs[2]);

		glColor4f(0.0, 1.0, 0.0, 0.5);
		glVertex3f(m->maxs[0], m->maxs[1], m->maxs[2]);

		glColor4f(0.0, 0.0, 1.0, 0.5);
		glVertex3f(m->maxs[0], m->maxs[1], m->mins[2]);
	}
	glEnd();

	glBegin(GL_POLYGON);
	{
		// Side 4.
		glColor4f(0.0, 1.0, 1.0, 0.25);
		glVertex3f(m->mins[0], m->maxs[1], m->mins[2]);

		glColor4f(1.0, 0.0, 0.0, 0.5);
		glVertex3f(m->mins[0], m->maxs[1], m->maxs[2]);

		glColor4f(0.0, 1.0, 0.0, 0.5);
		glVertex3f(m->maxs[0], m->maxs[1], m->maxs[2]);

		glColor4f(0.0, 0.0, 1.0, 0.5);
		glVertex3f(m->maxs[0], m->maxs[1], m->mins[2]);
	}
	glEnd();

	glEnable(GL_CULL_FACE);

	glPopAttrib();
}

void draw_face(dface_t *face)
{
	dedge_t *edge;
	dvertex_t *v;
	float vertex_color; 
	int e;
	int lindex;
	float s, t;
	float max_z = models[0].maxs[2];
	float min_z = models[0].mins[2];
	texture_t *texture = &textures[face->texinfo];
	float *normal = planes[face->planenum].normal;
	
	// Don't draw walls in orthogonal projection.
	if (!config.perspective && (normal[2] > -0.5) && (normal[2] < 0.5))
		return;

	// Enable a clipping plane, so that a percentage of the absolute top of the level isn't drawn.
	{
		double plane[4] = {0.0, 0.0, -1.0, 0.0};
		glPushMatrix();
   		glEnable(GL_CLIP_PLANE0);
		glTranslatef(0.0, 0.0, max_z * config.clip_height);
   		glClipPlane(GL_CLIP_PLANE0, plane);
		glPopMatrix();
	}

	// Set the normal of the face.
	glNormal3fv(normal);
	
	// Draw the face.
	glBegin(GL_POLYGON);
	{
		if (strstr(texture->name, "sky"))
			goto end;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture->texnum);

		for (e = 0; e < face->numedges; e++)
		{
			lindex = ledges[face->firstedge + e];

			if (lindex > 0)
			{					
				edge = &edges[lindex];
				v = &vertices[edge->v[0]];
			}
			else
			{
				edge = &edges[-lindex];
				v = &vertices[edge->v[1]];
			}

			vertex_color = fabs(v->point[2] / (max_z - min_z));
			glColor4f(vertex_color, vertex_color, vertex_color, 0.5);

			glTexCoord2d(0.0, 0.0);
			glTexCoord2d(0.0, 1.0);
			glTexCoord2d(1.0, 0.0);
			glTexCoord2d(1.0, 1.0);
			glVertex3fv(v->point);
		}
	}
end:
	glEnd();

	//if (calculate_face_area(face) == 0.0)
	//	return;
	
	glBegin(GL_LINES);
	{
		dvertex_t *v0;
		dvertex_t *v1;

		glColor3f(1.0, 1.0, 1.0);
		for (e = 0; e < face->numedges; e++)
		{
			lindex = ledges[face->firstedge + e];
			
			edge = (lindex > 0) ? &edges[lindex] : &edges[-lindex];
			v0 = &vertices[edge->v[0]];
			v1 = &vertices[edge->v[1]];

			vertex_color = fabs(v->point[2] / ((max_z - min_z) * 0.1f));
			glColor4f(vertex_color, vertex_color, vertex_color, 0.5);

			if (!is_edge_long_enough(v0, v1))
				continue;

			glVertex3fv(v0->point);
			glVertex3fv(v1->point);
		}
	}
	glEnd();
	

	glDisable(GL_CLIP_PLANE0);
}

void enable_vertical_clipping_plane()
{
	float max_z = models[0].maxs[2];
	float min_z = models[0].mins[2];
	double plane[4] = {0.0, 0.0, -1.0, 0.0};
	glPushMatrix();
	glEnable(GL_CLIP_PLANE0);
	glTranslatef(0.0, 0.0, max_z * config.clip_height);
	glClipPlane(GL_CLIP_PLANE0, plane);
	glPopMatrix();
}

void disable_vertical_clipping_plane()
{
	glDisable(GL_CLIP_PLANE0);
}

void draw_polygon(polygon_t *polygon)
{
	dedge_t *edge;
	dvertex_t *v;
	float vertex_color; 
	int i;
	int lindex;
	float s, t;
	float max_z = models[0].maxs[2];
	float min_z = models[0].mins[2];
//	texture_t *texture = &textures[face->texinfo];
	float *normal = polygon->plane->normal;
	
	// Set the normal of the face.
	glNormal3fv(normal);
	
	// Draw the face.
	glBegin(GL_POLYGON);
	{
		//if (strstr(texture->name, "sky"))
		//	break;

		//glEnable(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D, texture->texnum);

		for (i = 0; i < polygon->vertex_count; i++)
		{
			v = polygon->vertices[i];

			vertex_color = fabs(v->point[2] / (max_z - min_z));
			glColor4f(vertex_color, vertex_color, vertex_color, 0.5);
			glVertex3fv(v->point);
		}
	}
	glEnd();
	
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	{
		dvertex_t *v0;
		dvertex_t *v1;
		for (i = 0; i < polygon->vertex_count - 1; i++)
		{
			v0 = polygon->vertices[i];
			v1 = polygon->vertices[i+1];

			if (!is_edge_long_enough(v0, v1))
				continue;

			glVertex3fv(v0->point);
			glVertex3fv(v1->point);
		}
	}
	glEnd();
}

void draw_axis()
{
	float size = 525.0;
	//glDepthFunc(GL_ALWAYS);     // to avoid visual artifacts with grid lines
    //glDisable(GL_LIGHTING);

    // draw axis
    glLineWidth(3);
    glBegin(GL_LINES);
        glColor3f(1, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(size, 0, 0);
        glColor3f(0, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, size, 0);
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, size);
    glEnd();
    glLineWidth(1);

    // draw arrows(actually big square dots)
    glPointSize(5);
    glBegin(GL_POINTS);
        glColor3f(1, 0, 0);
        glVertex3f(size, 0, 0);
        glColor3f(0, 1, 0);
        glVertex3f(0, size, 0);
        glColor3f(0, 0, 1);
        glVertex3f(0, 0, size);
    glEnd();
    glPointSize(1);

    // restore default settings
    //glEnable(GL_LIGHTING);
    //glDepthFunc(GL_LEQUAL);
}

void place_camera()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	if (config.perspective)
	{
		float x = config.x;
		float y = config.y;
		float z = config.z;
		float cx = config.cx;
		float cy = config.cy;
		float cz = config.cz;
		gluPerspective(70.0, 1.0, 1.0, 5000.0);
		gluLookAt(x, y, z, cx, cy, cz, 0, 0.0, 1.0);
	}
	else
	{
		float x = config.x;
		float y = config.y;
		float zoom = config.zoom;
		glOrtho(0.0, zoom * 800, zoom * 600, 0, -99999, 99999);
		glTranslatef(x, y, -100.0);
		glRotatef(180.0, 0.0, 1.0, 0.0);
	}
}


