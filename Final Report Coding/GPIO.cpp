#include "GPIO.h"
extern "C"{
#include <bcm2835.h>
}
#include "constants.h"
#define PIN0 RPI_GPIO_P1_11 
#define PIN1 RPI_GPIO_P1_12
#define PIN2 RPI_GPIO_P1_13

int control(RESULT Decesion){
	if(!bcm2835_init())
		return 1;	
	
		//define output mode
	bcm2835_gpio_fsel(PIN0, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PIN2, BCM2835_GPIO_FSEL_OUTP);

		//Close all the GPIO 
	for(;;){
		bcm2835_gpio_write(PIN0,LOW);
		bcm2835_gpio_write(PIN1,LOW);
		bcm2835_gpio_write(PIN2,LOW);
	}
	for(;;){
		if(Decesion == TURNMIDDLE){
			bcm2835_gpio_write(PIN0,HIGH);
		}
		else if(Decesion == TURNLEFT){
		bcm2835_gpio_write(PIN1,HIGH);
		}
		else{
		bcm2835_gpio_write(PIN2,HIGH);
		}
	}
	return 0;
}
