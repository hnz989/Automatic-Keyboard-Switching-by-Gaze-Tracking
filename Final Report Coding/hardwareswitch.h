#ifndef HARDWARESWITCH_H
#define HARDWARESWITCH_H

#include "constants.h"
extern "C"{
#include <bcm2835.h>
}
int control(RESULT Decesion);

#endif
