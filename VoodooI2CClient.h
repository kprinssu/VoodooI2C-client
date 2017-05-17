#ifndef VOODOO_I2C_CLIENT
#define VOODOO_I2C_CLIENT

#include "csgesture-softc.h"

#define GESTURE_CTL_NAME "com.alexandred.VoodooI2C.GestureSocket"

enum gesture_socket_cmd_type {
    GESTURE_DATA,
    GESTURE_QUIT
};

struct gesture_socket_cmd {
    enum gesture_socket_cmd_type type;
    struct csgesture_softc gesture;
};

#endif