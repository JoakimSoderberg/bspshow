
#ifndef __INPUT_H__
#define __INPUT_H__


void on_mouse(int button, int state, int mouse_x, int mouse_y);
void on_mouse_move(int mouse_x, int mouse_y);
void on_keyboard(unsigned char key, int key_x, int key_y);
void on_special_key(int key, int key_x, int key_y);

#endif // __INPUT_H__
