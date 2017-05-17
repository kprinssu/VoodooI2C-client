#ifndef MOUSE_H
#define MOUSE_H

#include <ApplicationServices/ApplicationServices.h>
#include "CGEventAdditions.h"

CGPoint current_mouse_pos();
void move_mouse(CGPoint position);

void left_mouse_press(CGPoint position);
void left_mouse_drag(CGPoint position);
void left_mouse_release(CGPoint position);

void right_mouse_press(CGPoint position);
void right_mouse_drag(CGPoint position);
void right_mouse_release(CGPoint position);

void get_screen_resolution(int *width, int *height);

#endif