#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <wiringPi.h>

#define SPEAKER 19
#define LED 12


int myTone(int gpio){
	int duty = 50;
	
	pwmWrite(SPEAKER, duty);
	return 0;
}

void initMyTone(int gpio,int i){
	int range = 100, div;
	
	div = 19200000/(range*i);
	
	pwmSetRange(range);
	pwmSetClock(div);
}

int myToneReset(int gpio){
	pwmWrite(SPEAKER, 0);
	return 0;
}

int main(){
    int f = 330;
	wiringPiSetupGpio();
    
    pinMode(SPEAKER, PWM_OUTPUT);
    pinMode(LED, PWM_OUTPUT);
    pwmWrite(LED, 1024);


	pwmSetMode(PWM_MODE_MS);


	myToneReset(SPEAKER);
	
	return 0;
}
