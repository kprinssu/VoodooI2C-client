#ifndef UPDD_H
#define UPDD_H

#include "csgesture-softc.h"

void updd_start();
void updd_stop();
void inject_touch(struct csgesture_softc* sc);

#endif