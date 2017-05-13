#include "upddapi.h"
#include "updd.h"
#include "mouse.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

bool keep_running = true;
bool updd_connected = false;

static pthread_mutex_t inject_touch_lock;
static struct csgesture_softc sc_old;

static void TBCALL connected_callback(unsigned long context, struct _PointerEvent* data);

// connect to the UPDD driver
void updd_start() {
	TBApiRegisterEvent(0, 0, _EventConfiguration, connected_callback);

	TBApiOpen();

	for(;;) {
		if(updd_connected) break;

		usleep(100);
	}
}

void inject_touch(struct csgesture_softc* sc) {
	if(!updd_connected) return;

	pthread_mutex_lock(&inject_touch_lock);

	// get the first avaliable multi touch device
	HTBDEVICE device = TBApiGetRelativeDevice(0);

	CGPoint current_mps = current_mouse_pos();

	if(sc->x[1] != -1 && sc->y[1] != -1) {
		int dx = sc->x[1] + sc_old.x[1];
		int dy = sc->y[1] + sc_old.y[1];

		printf("Moving pos %d %d\n", (int)current_mps.x + dx, (int)current_mps.y + dy );
		

		//if(dx < 100 && dy < 100) {
		TBApiInjectTouch(device, (int)current_mps.x + dx, (int)current_mps.y + dy, 1, true);
		//}
		
		//TBApiInjectTouch(device, sc->x[1], sc->y[1], 1, true);
		//TBApiInjectTouch(device, sc->x[2], sc->y[2], 2, true);
		//TBApiInjectTouch(device, sc->x[3], sc->y[3], 3, true);
	} else {
		printf("Last position %f %f\n",current_mps.x, current_mps.y);
		TBApiInjectTouch(device, (int)current_mps.x, (int)current_mps.y, 1, false);
		//TBApiInjectTouch(device, sc_old.x[1], sc_old.y[1], 1, false);
		//TBApiInjectTouch(device, sc_old.x[2], sc_old.y[2], 2, false);
		//TBApiInjectTouch(device, sc_old.x[3], sc_old.y[3], 3, false);
	}


	memcpy(&sc_old, sc, sizeof(struct csgesture_softc));
	
	pthread_mutex_unlock(&inject_touch_lock);
}

// 
void updd_stop() {
	TBApiClose();
}

static void TBCALL connected_callback(unsigned long context, struct _PointerEvent* ev) {
	if (ev->pe.config.configEventType == CONFIG_EVENT_CONNECT) {
		updd_connected = true; 
	}
}
