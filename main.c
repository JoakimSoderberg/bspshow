#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
//#include <gl/gl.h>
//#include <gl/glu.h>
#include <gl/freeglut.h>

#include <math.h>
#include <time.h>

#include "bsp.h"

#if 1
size_t vertex_count = 0;
size_t face_count = 0;
size_t edge_count = 0;
size_t ledge_count = 0;
size_t model_count = 0;
size_t plane_count = 0;
size_t texture_count = 0;

dface_t *faces;
dedge_t *edges;
int *ledges;
dvertex_t *vertices;
dmodel_t *models;
dplane_t *planes;

// Texture related.
dmiptexlump_t *mip_header;
miptex_t *texturelist;
texinfo_t *textureinfos;

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

	/*
	glRotatef(angle, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);
	*/
	
	
	glColor3f(0.0, 1.0, 0.0);

	/*
	glBegin(GL_POINTS);
	{
		for (i = 0; i < vertex_count; i++)
		{
			glVertex3fv(vertices[i].point);
		}
	}
	glEnd();
	*/
	/*
	glBegin(GL_LINES);
	{
		dvertex_t *v0;
		dvertex_t *v1;

		for (i = 0; i < edge_count; i++)
		{
			v0 = &vertices[edges[i].v[0]];
			v1 = &vertices[edges[i].v[1]];

			glVertex3fv(v0->point);
			glVertex3fv(v1->point);
		}
	}
	glEnd();
	*/

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

typedef struct texture_s
{
	int width;
	int height;
	float s;
	float t;
	GLuint texnum;
} texture_t;


int read_textures()
{
	size_t i;

	mip_header = get_miptexture_header();
	texturelist = get_miptextures(mip_header, &texture_count);

	for (i = 0; i < texture_count; i++)
	{

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

	x = 300;
	y = 0;
	z = 2000;
	direction = 1.745f;
	updown = 0.61f;
	/*
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	
	{
		GLfloat ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color(0.2, 0.2, 0.2)
		 GLfloat lightColor0[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.5, 0.5)
		GLfloat lightPos0[] = {4.0f, 0.0f, 8.0f, 1.0f}; //Positioned at (4, 0, 8)
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glShadeModel(GL_SMOOTH);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);

		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	}*/

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
#else

GLfloat angle = 0.0;
void cube (void) {
    glRotatef(angle, 1.0, 0.0, 0.0);
    glRotatef(angle, 0.0, 1.0, 0.0);
    glRotatef(angle, 0.0, 0.0, 1.0);
    glColor3f(1.0, 0.0, 0.0);
    glutSolidCube(2);
}
void init (void) {
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
}
void display (void) {
    glClearColor (0.0,0.0,0.0,1.0);
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();  
    gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    cube();
    glutSwapBuffers();
    angle ++;
}
void reshape (int w, int h) {
    glViewport (0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode (GL_MODELVIEW);
}
int main (int argc, char **argv) {
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize (500, 500);
    glutInitWindowPosition (100, 100);
    glutCreateWindow ("A basic OpenGL Window");
    init ();
    glutDisplayFunc (display);
    glutIdleFunc (display);
    glutReshapeFunc (reshape);
    glutMainLoop ();
    return 0;
}
#endif