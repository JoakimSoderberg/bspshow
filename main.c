#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <gl/freeglut.h>

#include <math.h>
#include <time.h>

#include "bsp.h"

#define PI 3.14159

typedef struct texture_s
{
	char name[16];
	int width;
	int height;
	float s[3];
	float distS;
	float t[3];
	float distT;
	GLuint texnum;
	int is_loaded;
} texture_t;

dface_t *faces;
size_t face_count = 0;

dedge_t *edges;
size_t edge_count = 0;

int *ledges;
size_t ledge_count = 0;

dvertex_t *vertices;
size_t vertex_count = 0;

dmodel_t *models;
size_t model_count = 0;

dplane_t *planes;
size_t plane_count = 0;

texture_t *textures;
size_t texture_count = 0;

// Texture related.
dmiptexlump_t *mip_header;
miptex_t **mipmap_ptrs;
size_t mipmap_count = 0;

texinfo_t *textureinfos;
size_t texinfo_count = 0;

#define DotProduct(x,y)			((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2])

void vector_copy(float v0[3], float v1[3])
{
	memcpy(v0, v1, sizeof(float) * 3);
}

float cx, cy, cz;
float x, y, z;
float direction;
float updown;

int perspective = 0;
float zoom = 1;

void draw_bounding_box(dmodel_t *m)
{ 
	int render_type = GL_LINE;
	
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

void draw_text(float x, float y, float z, const char *format, ...)
{
	static char buf[512];
	const char *c;
	va_list args;

	buf[0] = NULL;
	
	glRasterPos3f(x, y,z);

	va_start(args, format);
	vsnprintf(buf, sizeof(buf), format, args);

	for (c = buf; *c != '\0'; c++) 
	{
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
	}
	
	va_end(args);
}

float clip_height = 1.0;

void draw_face(dface_t *face)
{
	dedge_t *edge;
	dvertex_t *v;
	int e;
	int lindex;
	float s, t;
	float max_z = models[0].maxs[2];
	float min_z = models[0].mins[2];
	texture_t *texture = &textures[face->texinfo];
	float *normal = planes[face->planenum].normal;
	
	// Don't draw walls in orthogonal projection.
	if (!perspective && (normal[2] > -0.5) && (normal[2] < 0.5))
		return;

	// Enable a clipping plane, so that a percentage of the absolute top of the level isn't drawn.
	{
		double plane[4] = {0.0, 0.0, -1.0, 0.0};
		glPushMatrix();
   		glEnable(GL_CLIP_PLANE0);
		glTranslatef(0.0, 0.0, max_z * clip_height);
   		glClipPlane(GL_CLIP_PLANE0, plane);
		glPopMatrix();
	}

	// Set the normal of the face.
	glNormal3fv(normal);

	// Draw the face.
	glBegin(GL_POLYGON);
	{
		if (strstr(texture->name, "sky"))
			return;

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

			{
				float c = fabs(v->point[2] / (max_z - min_z));
				glColor4f(c, c, c, 0.5);
			}
			glTexCoord2d(0.0, 0.0);
			glTexCoord2d(0.0, 1.0);
			glTexCoord2d(1.0, 0.0);
			glTexCoord2d(1.0, 1.0);
			glVertex3fv(v->point);
		}
	}
	glEnd();

	glDisable(GL_CLIP_PLANE0);
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
	
	if (perspective)
	{
		gluPerspective(70.0, 1.0, 1.0, 5000.0);
		gluLookAt(x, y, z, cx, cy, cz, 0, 0.0, 1.0);
	}
	else
	{
		glOrtho(0.0, zoom * 800, zoom * 600, 0, -99999, 99999);
		glTranslatef(x, y, -100.0);
	}
}

float angle;
void renderScene(void)
{
	size_t i;
	size_t j;

	place_camera();

	glMatrixMode(GL_MODELVIEW);
	
	glClearDepth(1.0);

	cx = 0.0;
	cy = 0.0;
	cz = 0.0;

	glClearColor(0.0,0.0,0.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	for (j = 0; j < model_count; j++)
	{
		for (i = models[j].firstface; i < (size_t)models[j].numfaces; i++)
		{
			draw_face(&faces[i]);
		}
	}

	glColor3f(1.0, 0.0, 0.0);
	draw_text(300.0, 300.0, 0.0, "(%f, %f)", x, y);

	draw_axis();

	draw_bounding_box(&models[0]);

	glPopMatrix();
	glutSwapBuffers();

	angle++;
}

void upload_texture(texture_t *t, const miptex_t *mip, const byte *texture_data, int mip_level)
{
	size_t i;
	size_t tex_index;
	int color_index;
	size_t width = mip->width / (int)pow(2.0, mip_level);
	size_t height = mip->height / (int)pow(2.0, mip_level);
	size_t original_size = width * height;
	size_t expanded_size = (original_size * 3);
	byte *expanded_data = (byte *)malloc(expanded_size);
	t->width = width;
	t->height = height;

	// Convert the image from indexed palette values to RGB values for each pixel.
	for (i = 0, tex_index = 0; i < expanded_size; i += 3, tex_index++)
	{
		color_index = texture_data[tex_index];
		memcpy(&expanded_data[i], &quake_pallete[color_index], 3);
	}

	glBindTexture(GL_TEXTURE_2D, t->texnum);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, mip_level, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, expanded_data);
	
	free(expanded_data);
}

int read_textures()
{
	size_t i;
	size_t mip_level;
	size_t texnum = 0;
	texture_t *t;
	miptex_t *mip;
	texinfo_t *texinf; 
	unsigned char *texture_data;

	// Read the texture data.
	byte *texdata = get_miptexture_lump_data();
	mipmap_ptrs	= get_miptextures(texdata, &mipmap_count);
	textureinfos = get_texinfos(&texinfo_count);

	// Allocate space for the texture list.
	textures = (texture_t *)calloc(texinfo_count, sizeof(texture_t));

	// Several faces can share a texinfo.
	// Go through each face, and get its texinfo. Only create a new texture_t
	// if the texture hasn't already been loaded.
	for (i = 0; i < face_count; i++)
	{
		texnum = faces[i].texinfo;

		if (!textures[texnum].is_loaded)
		{
			texinf = &textureinfos[texnum];

			t = &textures[texnum];
			t->texnum = texnum;

			vector_copy(t->s, texinf->vectorS);
			vector_copy(t->t, texinf->vectorT);
			t->distS = texinf->distS;
			t->distT = texinf->distT;

			// Get the pointer to the mipmap.
			mip = mipmap_ptrs[texinf->miptex];
			strcpy(t->name, mip->name);
			t->width = mip->width;
			t->height = mip->height;

			// Upload the texture data to OpenGL.
			for (mip_level = 0; mip_level < MIPLEVELS - 1; mip_level++)
			{
				texture_data = get_texture(texdata, mip, mip_level);
				upload_texture(t, mip, texture_data, mip_level);
				free(texture_data);
			}

			t->is_loaded = 1;
			texture_count++;
		}
	}

	return 1;
}

int read_bsp_data(const char *filename)
{
	if (!open_bsp(filename))
	{
		printf("Failed to open bsp: %s.\n", filename);
		return 0;
	}

	faces = get_faces(&face_count);
	edges = get_edges(&edge_count);
	ledges = get_ledges(&ledge_count);
	vertices = get_vertices(&vertex_count);
	models = get_models(&model_count);
	planes = get_planes(&plane_count);

	read_textures();

	return 1;
}

void init_lighting()
{
	GLfloat light_diffuse[4] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};

	glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
}

const int window_width = 800;
const int window_height = 600;
int left_mouse = 0;
int right_mouse = 0;
int down_mouse_x;
int down_mouse_y;

void on_mouse(int button, int state, int mouse_x, int mouse_y)
{
	right_mouse = ((state == GLUT_DOWN) && (button == GLUT_RIGHT_BUTTON));
	left_mouse = ((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON));
	down_mouse_x = mouse_x;
	down_mouse_y = mouse_y;
}

void on_mouse_move(int mouse_x, int mouse_y)
{
	if (right_mouse)
	{
		x = (float)(x - down_mouse_x);
		y = (window_height) - (float)(y - down_mouse_y);
	}

	down_mouse_x = mouse_x;
	down_mouse_y = mouse_y;
}

void on_keyboard(unsigned char key, int key_x, int key_y)
{
	if (key == 'p')
	{
		perspective = !perspective;
	}

	if (key == '+')
	{
		clip_height += 0.01f;
	}

	if (key == '-')
		clip_height -= 0.01f;
}

void on_special_key(int key, int key_x, int key_y)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:		x += 10.0f; break;
		case GLUT_KEY_RIGHT:	x += 10.0f; break;
		case GLUT_KEY_UP:		y += 10.0f; break;
		case GLUT_KEY_DOWN:		y -= 10.0f; break;
		case GLUT_KEY_PAGE_UP:	zoom += 0.1f; break;
		case GLUT_KEY_PAGE_DOWN: zoom -= 0.1f; break;
	}
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Not enough arguments\n");
		return -1;
	}

	if (!read_bsp_data(argv[1]))
		return -1;

	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(window_width, window_height);
	glutCreateWindow("SnowMen");
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	//glutMouseFunc(on_mouse);
	//glutMotionFunc(on_mouse_move);
	glutKeyboardFunc(on_keyboard);
	glutSpecialFunc(on_special_key);
	
	srand(time(NULL));

	x = 300;
	y = 0;
	z = 2000;
	direction = 1.745f;
	updown = 0.61f;
		
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(70.0, 1.0, 1.0, 5000.0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//init_lighting();

	glMatrixMode(GL_MODELVIEW);
	glRotatef(180.0, 0.0, 0.0, 1.0);
	
	glutMainLoop();
	
	// Cleanup.
	free(vertices);
	free(edges);
	free(ledges);
	free(models);

	return 0;
}

