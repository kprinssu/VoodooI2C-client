#include "upddapi.h"
#include "updd.h"

#include <unistd.h>

bool keep_running = true;
bool updd_connected = false;

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

	// get the first avaliable multi touch device
	HTBDEVICE device = TBApiGetRelativeDevice(0);
	printf("Injecting for %d %d\n", sc->x[0], sc->y[0]);
	if(sc->x[0] != -1 && sc->y[0] != -1) {
		TBApiInjectTouch(device, sc->x[0], sc->y[0], 0, true);
	}
	
	memcpy(&sc_old, sc, sizeof(struct csgesture_softc));
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