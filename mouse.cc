#include "mouse.h"
#include <mach/mach_time.h>

static inline uint64_t tl_uptime() {
	return mach_absolute_time();
}

void bounds_check(CGPoint *point) {
	if(point->x < 0) {
		point->x = 0;
	}

	if(point->y < 0) {
		point->y = 0;
	}
}

CGPoint current_mouse_pos() {
	CGEventRef const event = CGEventCreate(nil);
    const CGPoint point = CGEventGetLocation(event);
	CFRelease(event);

	return point;
}

void move_mouse(CGPoint position) {
	bounds_check(&position);

	CGEventRef move = CGEventCreateMouseEvent(
		NULL, kCGEventMouseMoved,
		position,
		kCGMouseButtonLeft // ignored
	);

	CGEventPost(kCGHIDEventTap, move);

	CFRelease(move);
}

void left_mouse_press(CGPoint position) {
	CGEventRef left_click = CGEventCreateMouseEvent(
		NULL, kCGEventLeftMouseDown,
		position,
		kCGMouseButtonLeft
	);
	printf("Mouse left press down\n");
	CGEventPost(kCGHIDEventTap, left_click);
	CFRelease(left_click);
}

void left_mouse_drag(CGPoint position) {
	bounds_check(&position);

	CGEventRef move = CGEventCreateMouseEvent(
		NULL, kCGEventLeftMouseDragged,
		position,
		kCGMouseButtonLeft // ignored
	);

	CGEventPost(kCGHIDEventTap, move);
	CFRelease(move);
}

void left_mouse_release(CGPoint position) {
	CGEventRef left_click = CGEventCreateMouseEvent(
		NULL, kCGEventLeftMouseUp,
		position,
		kCGMouseButtonLeft
	);
	printf("Mouse left press up\n");
	CGEventPost(kCGHIDEventTap, left_click);
	CFRelease(left_click);
}

void right_mouse_press(CGPoint position) {
	CGEventRef right_click = CGEventCreateMouseEvent(
		NULL, kCGEventRightMouseDown,
		position,
		kCGMouseButtonRight
	);
	printf("Mouse right press down\n");
	CGEventPost(kCGHIDEventTap, right_click);
	CFRelease(right_click);
}

void right_mouse_drag(CGPoint position) {
	bounds_check(&position);

	CGEventRef move = CGEventCreateMouseEvent(
		NULL, kCGEventRightMouseDragged,
		position,
		kCGMouseButtonLeft // ignored
	);

	CGEventPost(kCGHIDEventTap, move);
	CFRelease(move);
}

void right_mouse_release(CGPoint position) {
	CGEventRef right_click = CGEventCreateMouseEvent(
		NULL, kCGEventRightMouseUp,
		position,
		kCGMouseButtonRight
	);
	printf("Mouse right press up\n");
	CGEventPost(kCGHIDEventTap, right_click);
	CFRelease(right_click);
}

void get_screen_resolution(int *width, int *height) {
	CGDirectDisplayID main_display = CGMainDisplayID();

	*width = CGDisplayPixelsWide(main_display);
	*height = CGDisplayPixelsHigh(main_display);
}