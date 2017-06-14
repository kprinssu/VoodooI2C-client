#include "upddapi.h"
#include "updd.h"
#include "csgesture-softc.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>

bool keep_running = true;
bool updd_connected = false;

#define MAX_FINGERS 5
#define SCREEN_NORMALISE(value, physical_size, screen_size) ((float)value / physical_size) * screen_size

static struct csgesture_softc sc_old;
static bool gestures[MAX_FINGERS];

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


void normalised_inject_touch(int x, int y, int resx, int resy, int finger, bool touching) {
	HTBDEVICE device = TBApiGetRelativeDevice(0);
	
	float x1 = (x * 1.0f / resx) * 3264;
	float y2 = (y * 1.0f / resy) * 1856; 

	printf("Touch(%d | %d): %f %f\n", finger, touching, x1, y2 );

	TBApiInjectTouch(device, (int)x1, (int)y2, finger, touching);
}

void inject_touch(struct csgesture_softc* sc) {
	if(!updd_connected) return;

	// start a new gesture
	if(sc_old.x[0] == -1 && sc_old.y[0] == -1) {

		for(int i = 0; i < MAX_FINGERS; i++) {
			gestures[i] = false;
		}

		memcpy(&sc_old, sc, sizeof(struct csgesture_softc));

		delete sc;

		return;
	}

	// inject touches
	if(sc->x[0] != -1 && sc->y[0] != -1) {
		for(int i = 0; i < MAX_FINGERS; i++) {
			if(sc->x[i] != -1) {
				gestures[i] = true;
				normalised_inject_touch(sc->x[i], sc->y[i], sc->resx, sc->resy, i, true);
			} else if(gestures[i]) {
				normalised_inject_touch(sc_old.x[i], sc_old.y[i], sc->resx, sc->resy, i, true);
			}
		}
	} else {
		for(int i = 0; i < MAX_FINGERS; i++) {
			if(gestures[i]) {
				normalised_inject_touch(sc_old.x[i], sc_old.y[i], sc->resx, sc->resy, i, false);
			}
		}
	}

	memcpy(&sc_old, sc, sizeof(struct csgesture_softc));

	delete sc;
}

void updd_stop() {
	TBApiUnregisterEvent(connected_callback);
	TBApiClose();
}

static void TBCALL connected_callback(unsigned long context, struct _PointerEvent* ev) {
	if (ev->pe.config.configEventType == CONFIG_EVENT_CONNECT) {
		updd_connected = true; 
	}
}
