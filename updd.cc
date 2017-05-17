#include "upddapi.h"
#include "updd.h"
#include "mouse.h"
#include "csgesture-softc.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <mutex>

#ifdef __cplusplus
extern "C" {
	#include "TouchEvents.h"
}
#endif

using namespace std;

bool keep_running = true;
bool updd_connected = false;

#define MAX_FINGERS 5
#define SCREEN_NORMALISE(value, physical_size, screen_size) ((float)value / physical_size) * screen_size

static mutex inject_touch_lock;

static struct csgesture_softc sc_old;
static CGPoint start_touch;
static bool multi_touch = false;
static void TBCALL connected_callback(unsigned long context, struct _PointerEvent* data);

// connect to the UPDD driver
void updd_start() {
	TBApiRegisterEvent(0, 0, _EventConfiguration, connected_callback);

	TBApiOpen();

	for(;;) {
		if(updd_connected) break;

		usleep(100);
	}

	sc_old.x[0] = -1;
	sc_old.y[0] = -1;
}

/*
void normalised_inject_touch(int x, int y, int resx, int resy, int finger, bool touching) {
	HTBDEVICE device = TBApiGetRelativeDevice(0);
	//int screen_width, screen_height;
	//get_screen_resolution(&screen_width, &screen_height);

	
	inject_touch_lock.lock();
	float x1 = (x * 1.0f / resx) * 1600;
	float y2 = (y * 1.0f / resy) * 900; 

	printf("Touch(%d): %f %f\n", finger, x1, y2 );

	
	TBApiInjectTouch(device, x, y , finger, touching);
	inject_touch_lock.unlock();
} */

void inject_touch(struct csgesture_softc* sc) {
	if(!updd_connected) return;

	// get the first avaliable multi touch device
	start_touch = current_mouse_pos();

	
	if(sc_old.x[0] == -1 && sc_old.y[0] == -1) {
		
		//start_touch = current_mouse_pos();
		//printf("Starting new touch (%f %f)\n", start_touch.x, start_touch.y);
		memcpy(&sc_old, sc, sizeof(struct csgesture_softc));
		
		//inject_touch_lock.unlock();
		delete sc;

		return;
	}

	bool pressed = false;
	int nfingers = 0;
    for (int i = 0;i < MAX_FINGERS;i++) {
        if (sc->x[i] != -1)
            nfingers++;
    }

    int old_nfingers = 0;
    for (int i = 0;i < MAX_FINGERS;i++) {
        if (sc_old.x[i] != -1)
            old_nfingers++;
    }

    int dx = sc->x[0] - sc_old.x[0];
    int dy = sc->y[0] - sc_old.y[0];

    int old_dx = sc_old.x[0] - sc_old.lastx[0];
    int old_dy = sc_old.y[0] - sc_old.lasty[0];

   //printf("Data %d %d | %d %d\n", dx, dy, old_dx, old_dy);

    start_touch.x += (dx);
    start_touch.y += (dy);

	if(!sc_old.buttondown && sc->buttondown) {
		switch(nfingers) {
			case 1: {
				left_mouse_press(start_touch);
				break;
			}
			case 2: {
				right_mouse_press(start_touch);
				break;
			}
		}
	} else if(sc_old.buttondown && !sc->buttondown) {
		switch(nfingers) {
			case 1: {
				left_mouse_release(start_touch);
				break;
			}
			case 2: {
				right_mouse_release(start_touch);
				break;
			}
		}
	} else if(sc_old.buttondown && sc->buttondown) {
		switch(nfingers) {
			case 1:
				left_mouse_drag(start_touch);
				break;
			case 2:
				right_mouse_drag(start_touch);
				break;
		}
		
		pressed = true;
	}

	

	if(!pressed && sc->x[0] != -1 && sc->y[0] != -1) {
		//printf("Test %d %d\n", sc->x[2], sc->y[2]);
		
		//TBApiInjectTouch(device, start_touch.x + dx, start_touch.y + dx, 0, true);
		//TBApiInjectTouch(device, sc->x[1], sc->y[1], 1, true);
		//TBApiInjectTouch(device, sc->x[2], sc->y[2], 2, true);
		//printf("Start %f %f\n", sc->x[0] / (float)sc->resx * 1600, SCREEN_NORMALISE(sc->x[0], sc->resx, 900));

		bool test = sc_old.panningActive;
		if(dx != 0 && dy != 0) {
			move_mouse(start_touch);
			sc->panningActive = true;
		}

		sc->panningActive = sc->panningActive || sc_old.panningActive;

		printf("Move mouse: %d->%d %f %f\n", test, sc_old.panningActive, start_touch.x, start_touch.y);

		//TBApiInjectTouch(device, start_touch.x + dx, start_touch.y + dx, 0, true);
		//TBApiInjectTouch(device, sc->x[3], sc->y[3], 3, true);
	} else if(!pressed) {
		//CGPoint current_mps = current_mouse_pos();
		//printf("Last position %f %f\n",current_mps.x, current_mps.y);
		//TBApiInjectTouch(device, (int)current_mps.x, (int)current_mps.y, 0, false);
		//TBApiInjectTouch(device, sc_old.x[1], sc_old.y[1], 1, false);
		//TBApiInjectTouch(device, sc_old.x[2], sc_old.y[2], 2, false);
		//TBApiInjectTouch(device, sc_old.x[3], sc_old.y[3], 3, false);
		//printf("Stopped touch (%f %f)\n", current_mps.x, current_mps.y);

		if(old_dx == 0 && old_dy == 0 && !sc_old.panningActive) {
			start_touch.x -= (dx);
			start_touch.y -= (dy);

			switch(old_nfingers) {
				case 1: {
					left_mouse_press(start_touch);
					usleep(1000);
					left_mouse_release(start_touch);
					break;
				}
				case 2: {
					right_mouse_press(start_touch);
					usleep(1000);
					right_mouse_release(start_touch);
					break;
				}
			}
		}

		sc_old.mouseDownDueToTap = false;
		sc->panningActive = false;

		sc_old.x[0] = -1;
		sc_old.y[0] = -1;

		//start_touch = current_mouse_pos();
	}
/*
	// 3 finger
	//printf("Touch %d %d\n", sc->x[2], sc->y[2]);
	if(sc->x[2] != -1 && sc->y[2] != -1) {
		multi_touch = true;
		normalised_inject_touch(sc->x[2], sc->y[2], sc->resx, sc->resy, 2, true);
	} else if(multi_touch && sc_old.x[2] == -1 && sc_old.y[2] == -1) {
		multi_touch = false;
		normalised_inject_touch(sc_old.x[2], sc_old.y[2], sc->resx, sc->resy, 2, false);
	} 

	// first finger
	if(multi_touch && sc->x[0] != -1 && sc->y[0] != -1) {
		normalised_inject_touch(sc->x[0], sc->y[0], sc->resx, sc->resy, 0, true);
	} else if(multi_touch && sc_old.x[0] != -1 && sc_old.y[0] != -1) {
		normalised_inject_touch(sc_old.x[0], sc_old.y[0], sc->resx, sc->resy, 0, false);
	}

	// second finger
	if(multi_touch && sc->x[1] != -1 && sc->y[1] != -1) {
		normalised_inject_touch(sc->x[1], sc->y[1], sc->resx, sc->resy, 1, true);
	} else if(multi_touch && multi_touch && sc_old.x[1] != -1 && sc_old.y[1] != -1) {
		normalised_inject_touch(sc_old.x[1], sc_old.y[1], sc->resx, sc->resy, 1, false);
	}

	// fourth finger
	if(multi_touch && sc->x[3] != -1 && sc->y[3] != -1) {
		normalised_inject_touch(sc->x[3], sc->y[3], sc->resx, sc->resy, 3, true);
	} else if(multi_touch && sc_old.x[3] != -1 && sc_old.y[3] != -1) {
		normalised_inject_touch(sc_old.x[3], sc_old.y[3], sc->resx, sc->resy, 3, false);
	}

	// fifth finger
	if(multi_touch && sc->x[4] != -1 && sc->y[4] != -1) {
		normalised_inject_touch(sc->x[4], sc->y[4], sc->resx, sc->resy, 4, true);
	} else if(multi_touch && sc_old.x[4] != -1 && sc_old.y[4] != -1) {
		normalised_inject_touch(sc_old.x[4], sc_old.y[4], sc->resx, sc->resy, 4, false);
	} */

	inject_touch_lock.lock();
	memcpy(sc->lastx, sc_old.x, 15);
	memcpy(sc->lasty, sc_old.y, 15);
	//printf("Res: %d %d\n", sc->resx, sc->resy);
	memcpy(&sc_old, sc, sizeof(struct csgesture_softc));
	//printf("Res Old: %d %d\n", sc_old.resx, sc_old.resy );
	inject_touch_lock.unlock();

	delete sc;
	//pthread_mutex_unlock(&inject_touch_lock);
}

// 
void updd_stop() {
	TBApiUnregisterEvent(connected_callback);
	TBApiClose();
}

static void TBCALL connected_callback(unsigned long context, struct _PointerEvent* ev) {
	if (ev->pe.config.configEventType == CONFIG_EVENT_CONNECT) {
		updd_connected = true; 
	}
}
