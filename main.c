#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <gl/freeglut.h>

#include <math.h>
#include <time.h>

#include "bsp.h"
#include "bsphelper.h"
#include "utils.h"

#define PI 3.14159

typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec5_t[5];

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

config_t config;

int init_config()
{
	config.window_width = 800;
	config.window_height = 600;
	config.zoom = 1.0;
	config.perspective = 0;
	config.clip_height = 1.0;

	return 1;
}


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

float calculate_face_area(dface_t *face)
{
	float total_area = 222220.0;
	/*float area = 0.0;
	int e;
	int lindex;
	dedge_t *edge;
	dvertex_t *first;
	dvertex_t *v0;
	dvertex_t *v1;
	float aa[3];
	float bb[3];
	float cross[3];
	
	lindex = ledges[face->firstedge];
	edge = (lindex > 0) ? &edges[lindex] : &edges[-lindex];
	first = &vertices[edge->v[0]];

	for (e = 1; e < face->firstedge; e++)
	{
		lindex = ledges[face->firstedge + e];

		edge = (lindex > 0) ? &edges[lindex] : &edges[-lindex];

		if (lindex > 0)
		{
			v0 = &vertices[edge->v[0]];
			v1 = &vertices[edge->v[1]];
		}
		else
		{
			v0 = &vertices[edge->v[1]];
			v1 = &vertices[edge->v[0]];
		}

		VectorSubtract(v0->point, first->point, aa);
		VectorSubtract(v1->point, first->point, bb);
		CrossProduct(aa, bb, cross);

		area = vector_length(cross);
		total_area += area;
	}

	return total_area;*/
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

	if (calculate_face_area(face) == 0.0)
		return;
	
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

			{
				float d[3];
				float edge_length;
				VectorSubtract(v0->point, v1->point, d);

				edge_length = sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);

				if (edge_length < config.min_edge_length)
					continue;
			}

			glVertex3fv(v0->point);
			glVertex3fv(v1->point);
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

float angle;
void renderScene(void)
{	
	size_t i;
	size_t j;

	place_camera();

	glMatrixMode(GL_MODELVIEW);
	
	glClearDepth(1.0);

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
	draw_text(300.0, 300.0, 0.0, "(%2.1f, %2.1f) zoom %2.2f", config.x, config.y, config.zoom);

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

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexImage2D(GL_TEXTURE_2D, mip_level, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, expanded_data);
	
	/*
	if (t->texnum == 24)
	{
		FILE *f = fopen("texture.raw", "wb");
		fwrite(expanded_data, 1, expanded_size, f);
		fclose(f);
		exit(0);
	}*/

	printf("Uploaded texture %d miplevel %d\n", t->texnum, mip_level);

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
		texinf = &textureinfos[faces[i].texinfo];
		texnum = texinf->miptex;

		if (texnum == 0)
			continue;

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
			for (mip_level = 0; mip_level < MIPLEVELS; mip_level++)
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

void on_mouse(int button, int state, int mouse_x, int mouse_y)
{
	config.right_mouse = ((state == GLUT_DOWN) && (button == GLUT_RIGHT_BUTTON));
	config.left_mouse = ((state == GLUT_DOWN) && (button == GLUT_LEFT_BUTTON));
	config.down_mouse_x = mouse_x;
	config.down_mouse_y = mouse_y;
}

void on_mouse_move(int mouse_x, int mouse_y)
{
	if (config.right_mouse)
	{
		config.x = (float)(config.x - config.down_mouse_x);
		config.y = (config.window_height) - (float)(config.y - config.down_mouse_y);
	}

	config.down_mouse_x = mouse_x;
	config.down_mouse_y = mouse_y;
}

void on_keyboard(unsigned char key, int key_x, int key_y)
{
	switch (key)
	{
		case 'p':	config.perspective = !config.perspective; break;
		case '+':	config.clip_height += 0.01f; break;
		case '-':	config.clip_height -= 0.01f; break;
		case 'w':	config.min_edge_length += 0.1f; break;
		case 's':	config.min_edge_length -= 0.1f; break;
		case 'e':	config.min_area += 1.0f; break;
		case 'd':	config.min_area -= 1.0f; break;
	}
}

void on_special_key(int key, int key_x, int key_y)
{
	switch (key)
	{
		case GLUT_KEY_LEFT:		config.x += 10.0f; break;
		case GLUT_KEY_RIGHT:	config.x -= 10.0f; break;
		case GLUT_KEY_UP:		config.y += 10.0f; break;
		case GLUT_KEY_DOWN:		config.y -= 10.0f; break;
		case GLUT_KEY_PAGE_UP:	config.zoom += 0.1f; break;
		case GLUT_KEY_PAGE_DOWN: config.zoom -= 0.1f; break;
	}
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Not enough arguments\n");
		return -1;
	}

	init_config();

	if (!read_bsp_data(argv[1]))
		return -1;

	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(config.window_width, config.window_height);
	glutCreateWindow("SnowMen");
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	//glutMouseFunc(on_mouse);
	//glutMotionFunc(on_mouse_move);
	glutKeyboardFunc(on_keyboard);
	glutSpecialFunc(on_special_key);
	
	srand(time(NULL));
	
	config.x = 300;
	config.y = 0;
	config.z = 2000;
	config.direction = 1.745f;
	config.updown = 0.61f;
		
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(70.0, 1.0, 1.0, 5000.0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//init_lighting();

	{
		float *maxs = models[0].maxs;
		float *mins = models[0].mins;
		float qsize_x = maxs[0] - mins[0];
		float qsize_y = maxs[1] - mins[1];

		printf("   Max      Min\n");
		printf("x %2.2f     %2.2f\n", maxs[0], mins[0]);
		printf("y %2.2f     %2.2f\n", maxs[1], mins[1]);
		printf("z %2.2f     %2.2f\n", maxs[2], mins[2]);
		config.x = models[0].maxs[0];
		config.y = -models[0].mins[1];

		if (qsize_x < qsize_y)
		{
			config.zoom = (qsize_y + config.window_height) / (float)config.window_width;
		}
		else
		{
			config.zoom = (qsize_x + config.window_width) / (float)config.window_height;
		}
		
	}

	glMatrixMode(GL_MODELVIEW);

	glutMainLoop();
	
	// Cleanup.
	free(vertices);
	free(edges);
	free(ledges);
	free(models);

	return 0;
}

