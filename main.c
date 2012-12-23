#include "bspshow_config.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef WITH_FREEGLUT
#include <gl/freeglut.h>
#else
#include <glut.h>
#endif

#include <math.h>
#include <time.h>

#include "bsp.h"
#include "utils.h"
#include "input.h"

#include "draw.h"
#include "bsphelper.h"
#include "config.h"

#define PI 3.14159

typedef float vec_t;
typedef vec_t vec3_t[3];
typedef vec_t vec5_t[5];

config_t config;
bsp_t bsp;
polygon_t *polygons;
size_t polygon_count;

int init_config()
{
	config.window_width = 800;
	config.window_height = 600;
	config.zoom = 1.0;
	config.perspective = 0;
	config.clip_height = 1.0;

	return 1;
}

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

	// Enable a clipping plane, so that a percentage of the absolute top of the level isn't drawn.
	enable_vertical_clipping_plane(&bsp);

	for (i = 0; i < polygon_count; i++)
	{
		float *normal = polygons[i].plane->normal;

		// Don't draw walls in orthogonal projection.
		if (!config.perspective && (normal[2] > -0.5) && (normal[2] < 0.5))
			continue;

		draw_polygon(&bsp, &polygons[i]);
	}

	disable_vertical_clipping_plane();

	glColor3f(1.0, 0.0, 0.0);
	draw_text(300.0, 300.0, 0.0, "(%2.1f, %2.1f) zoom %2.2f", config.x, config.y, config.zoom);

	draw_axis();

	draw_bounding_box(&bsp.models[0]);

	glPopMatrix();
	glutSwapBuffers();
}

void init_lighting()
{
	GLfloat light_diffuse[4] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};

	glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    glEnable (GL_LIGHT0);
}

int main(int argc, char **argv)
{
	printf("BSP Show v%s\n", BSPSHOW_VERSION);

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s filename.bsp\n", argv[0]);
		fprintf(stderr, "Not enough arguments!\n");
		return -1;
	}

	init_config();
	
	if (bsp_read_file(argv[1], &bsp))
	{
		fprintf(stderr, "Failed to read BSP\n");
		return -1;
	}

	polygons = bsp_build_polygon_list(&bsp, &polygon_count);

	glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(config.window_width, config.window_height);
	glutCreateWindow("BSP Show");
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
		float *maxs = bsp.models[0].maxs;
		float *mins = bsp.models[0].mins;
		float qsize_x = maxs[0] - mins[0];
		float qsize_y = maxs[1] - mins[1];

		printf("   Max      Min\n");
		printf("x %2.2f     %2.2f\n", maxs[0], mins[0]);
		printf("y %2.2f     %2.2f\n", maxs[1], mins[1]);
		printf("z %2.2f     %2.2f\n", maxs[2], mins[2]);
		config.x = bsp.models[0].maxs[0];
		config.y = -bsp.models[0].mins[1];

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
	bsp_destroy(bsp);

	return 0;
}

