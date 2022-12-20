#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <wiringPi.h>

#define SPEAKER 19
#define LED 12

int a = 0;

int myTone(int gpio){
	int duty = 50;
	
	pwmWrite(SPEAKER, duty);
	
	pwmWrite(LED, a);
	a = (a>=1000)?(0):(a+500);

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
    
	pinMode(LED, PWM_OUTPUT);
    pinMode(SPEAKER, PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);


	for(int i = 0; i < 200; i++){
		f = (f == 330)?(220):(330);

		initMyTone(SPEAKER, f); 
		myTone(SPEAKER);
        delay(100);
	}

	myToneReset(SPEAKER);
	
	return 0;
}
