#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <sys/stat.h> 
#include <mqueue.h>
#include <time.h>
#include <string.h>

#define TRIG 15
#define ECHO 18
#define BUFSIZE 32
#define PERIOD 1000

const char* mq_name_dis = "/mq_dis";

void initMicro();
void writeTrig();
double readEcho();


int main(){
    char buf[BUFSIZE] = "0";
    mqd_t mq_dis;
	int i2c_fd;
    double temp;
    double distance = 0;
    int n = 0;

    mq_dis = mq_open(mq_name_dis, O_WRONLY);

    //wiringPi
	if(wiringPiSetupGpio() < 0 ){
		printf("wiringPiSetup() is failed\n");
		return 1;
	}

    //Distance
    initMicro();

    while(1){
        n = 0;
        for(int i = 0; i < 5; i++){
            writeTrig();
            distance = readEcho();
            if(distance > 6.0){
                n++;
            }
            //printf("distance : %0.2lf\n",distance);
            delay(PERIOD-200);
        }
        
        if(n >= 5){
            sprintf(buf, "%d", 1);
        }
        else{
            sprintf(buf, "%d", 0);
        }
        mq_send(mq_dis, buf, strlen(buf), 0);
    }

    mq_close(mq_dis);
	return 0;	
}
    



void initMicro(){
	pinMode(TRIG, OUTPUT);
	pinMode(ECHO , INPUT);
}
void writeTrig(){
	digitalWrite(TRIG, LOW);
    delay(200);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
}
double readEcho(){
	int start, end;
	while (digitalRead(ECHO) == 0);
    start = micros();
    while (digitalRead(ECHO) == 1);
    end = micros();
    return (end - start) / 29. / 2.;
}



