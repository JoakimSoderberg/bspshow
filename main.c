#include "bspshow_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
bsp_helper_t bsph;
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

static void write_targa(const char *filename, const GLubyte *buffer, int width, int height)
{
	FILE *f = fopen( filename, "w" );

	if (f) 
	{
		int i, x, y;
		const GLubyte *ptr = buffer;
		fputc (0x00, f);	/* ID Length, 0 => No ID	*/
		fputc (0x00, f);	/* Color Map Type, 0 => No color map included	*/
		fputc (0x02, f);	/* Image Type, 2 => Uncompressed, True-color Image */
		fputc (0x00, f);	/* Next five bytes are about the color map entries */
		fputc (0x00, f);	/* 2 bytes Index, 2 bytes length, 1 byte size */
		fputc (0x00, f);
		fputc (0x00, f);
		fputc (0x00, f);
		fputc (0x00, f);	/* X-origin of Image	*/
		fputc (0x00, f);
		fputc (0x00, f);	/* Y-origin of Image	*/
		fputc (0x00, f);
		fputc (width & 0xff, f);      /* Image Width	*/
		fputc ((width>>8) & 0xff, f);
		fputc (height & 0xff, f);     /* Image Height	*/
		fputc ((height>>8) & 0xff, f);
		fputc (0x18, f);		/* Pixel Depth, 0x18 => 24 Bits	*/
		fputc (0x20, f);		/* Image Descriptor	*/
		fclose(f);
		f = fopen( filename, "ab" );  /* reopen in binary append mode */
		for (y=height-1; y>=0; y--) 
		{
			for (x=0; x<width; x++) 
			{
				i = (y*width + x) * 3;
				fputc(ptr[i+2], f); /* write blue */
				fputc(ptr[i+1], f); /* write green */
				fputc(ptr[i], f);   /* write red */
			}
      }
      fclose(f);
	}
}


int parse_cmdline(int argc, char **argv)
{
	size_t i;

	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s filename.bsp\n", argv[0]);
		fprintf(stderr, "Not enough arguments!\n");
		return -1;
	}

	config.filename = NULL;

	for (i = 1; i < argc; i++)
	{
		char *s = argv[i];
		size_t slen = strlen(s);

		if (slen == 0)
			continue;

		if (s[0] == '-')
		{
			if (slen <= 1)
				continue;

			switch (s[1])
			{
				case 't':
					config.list_textures = 1;
					break;
				default:
					fprintf(stderr, "Unknown switch '%c'\n", s[1]);
					break;
			}
		}
		else
		{
			config.filename = s;
		}
	}

	if (!config.filename)
	{
		return -1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	printf("BSP Show v%s\n", BSPSHOW_VERSION);

	init_config();

	if (parse_cmdline(argc, argv))
	{
		return -1;
	}
	
	if (bsp_read_file(config.filename, &bsp))
	{
		fprintf(stderr, "Failed to read BSP\n");
		return -1;
	}

	printf("%s\n", bsp.entity_data);

	// List all textures.
	if (config.list_textures)
	{
		size_t i;
		texinfo_t *t;
		miptex_t *mip;

		for (i = 0; i < bsp.mipmap_count; i++)
		{
			mip = bsp.mip_list[i];

			printf("Texture %-2zu %15s (%-2dx%-2d)\n", i, mip->name, mip->width, mip->height);

			#if 0
			// Write the texture to file.
			if (i == 6)
			{
				char mipname[1024];
				byte *data = bsp_get_texture(&bsp, i, 0);
				byte *expanded_data = convert_8bit_to_24bit(mip, data, 0);

				strlcpy(mipname, mip->name, sizeof(mipname));
				strlcat(mipname, ".tga", sizeof(mipname));

				write_targa(mipname, (GLubyte *)expanded_data, mip->width, mip->height);
				free(data);
			}
			#endif
		}
		return;
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

