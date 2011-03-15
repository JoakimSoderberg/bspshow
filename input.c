
#include <gl/freeglut.h>
#include "input.h"
#include "config.h"

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

