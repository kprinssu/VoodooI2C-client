#include "mouse.h"

CGPoint current_mouse_pos() {
	CGEventRef const event = CGEventCreate(nil);
    const CGPoint point = CGEventGetLocation(event);
	CFRelease(event);

	return point;
}