#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
//#include <gl/gl.h>
//#include <gl/glu.h>
#include <gl/freeglut.h>

#include <math.h>
#include <time.h>

#include "bsp.h"

typedef struct texture_s
{
	int width;
	int height;
	float s[3];
	float t[3];
	GLuint texnum;
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

void vector_copy(float v0[3], float v1[3])
{
	memcpy(v0, v1, sizeof(float) * 3);
}

float cx, cy, cz;
float x, y, z;
float direction;
float updown;

void draw_face(dface_t *face)
{
	dedge_t *edge;
	dvertex_t *v;
	int e;
	int lindex;
	
	// Set the normal of the face.
	glNormal3fv(planes[face->planenum].normal);

	// Draw the face.
	glBegin(GL_POLYGON);

	for (e = 0; e < face->numedges; e ++)
	{
		lindex = ledges[face->firstedge + e];

		if (lindex > 0)
		{					
			edge = &edges[lindex];
			v = &vertices[edge->v[0]];
			glVertex3fv(v->point);
		}
		else
		{
			edge = &edges[-lindex];
			v = &vertices[edge->v[1]];
			glVertex3fv(v->point);
		}
	}

	glEnd();
}

float angle;
void renderScene(void) 
{
	size_t i;
	size_t j;
	glMatrixMode(GL_MODELVIEW);
	
	glClearDepth(1.0);

	//cx = x + cos(direction) * cos(updown);
	//cy = y + sin(direction) * cos(updown);
	//cz = z + sin(updown);
	cx = -50.0;
	cy = -50.0;
	cz = 50.0;

	glClearColor (0.0,0.0,0.0,1.0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(x, y, z, cx, cy, cz, 0, 0.0, 1.0);

	glColor3f(0.0, 1.0, 0.0);
	
	for (j = 0; j < model_count; j++)
	{
		for (i = models[j].firstface; i < (size_t)models[j].numfaces; i++)
		{
			//glColor3f(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX);
			draw_face(&faces[i]);
		}
	}

	glPopMatrix();
	glutSwapBuffers();

	angle++;
}


void upload_texture(texture_t *t, const miptex_t *mip, const byte *texture_data, int mip_level)
{
	size_t i, j;
	size_t tex_index;
	int color_index;
	size_t width = mip->width / (int)pow(2.0, mip_level);
	size_t height = mip->height / (int)pow(2.0, mip_level);
	size_t original_size = width * height;
	size_t expanded_size = (original_size * 3);
	byte *expanded_data = (byte *)malloc(expanded_size);
	t->width = width;
	t->height = height;

	for (i = 0, tex_index = 0; i < expanded_size; i += 3, tex_index++)
	{
		color_index = texture_data[tex_index];

		for (j = 0; j < 3; j++)
		{
			expanded_data[i + j] = quake_pallete[color_index][j];
		}
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

int texture_loaded(size_t texnum)
{
	size_t i;

	for (i = 0; i < texture_count; i++)
	{
		if (textures[i].texnum == texnum)
			return 1;
	}

	return 0;
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
	mipmap_ptrs = get_miptextures(texdata, &mipmap_count);
	textureinfos = get_texinfos(&texinfo_count);

	// Allocate space for the texture list.
	textures = (texture_t *)calloc(texinfo_count, sizeof(texture_t));
	memset(textures, -1, sizeof(texture_t) * texinfo_count);

	// Several faces can share a texinfo.
	// Go through each face, and get its texinfo. Only create a new texture_t
	// if the texture hasn't already been loaded.
	for (i = 0; i < face_count; i++)
	{
		texnum = faces[i].texinfo;

		if (!texture_loaded(texnum))
		{
			texinf = &textureinfos[texnum];

			t = &textures[texture_count++];
			t->texnum = texnum;
			
			vector_copy(t->s, texinf->vectorS);
			vector_copy(t->t, texinf->vectorT);

			// Get the pointer to the mipmap.
			mip = mipmap_ptrs[texinf->miptex];

			// Upload the texture data to OpenGL.
			for (mip_level = 0; mip_level < MIPLEVELS - 1; mip_level++)
			{
				texture_data = get_texture(texdata, mip, mip_level);
				upload_texture(t, mip, texture_data, mip_level);
				free(texture_data);
			}
		}
	}

	return 1;
}


int read_bsp_data()
{
	if (!open_bsp("aerowalk.bsp"))
	{
		printf("Failed to open bsp.\n");
		return 0;
	}

	faces = get_faces(&face_count);
	edges = get_edges(&edge_count);
	ledges = get_ledges(&ledge_count);
	vertices = get_vertices(&vertex_count);
	models = get_models(&model_count);
	planes = get_planes(&plane_count);

	return 1;
}

void init_lighting()
{
	GLfloat light_diffuse[4] = {1.0, 1.0, 1.0, 1.0};
	//GLfloat light_position[4] = {1000.0, 1000.0, 2000.0, 0.0};
	GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};

	glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
}

int main(int argc, char **argv)
{
	
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800,600);
	glutCreateWindow("SnowMen");
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	

	srand(time(NULL));


	if (!read_bsp_data())
		return -1;

	read_textures();

	x = 300;
	y = 0;
	z = 2000;
	direction = 1.745f;
	updown = 0.61f;
	
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, 1.0, 1.0, 5000.0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	init_lighting();

	
	glutMainLoop();
	
	free(vertices);
	free(edges);
	free(ledges);
	free(models);

	return 0;
}

