#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <math.h>
#include <sys/stat.h> 
#include <mqueue.h>
#include <time.h>
#include <string.h>
#include <wiringPiI2C.h>

#define BUFSIZE 32
#define SLAVE_ADDR_01 0x48
#define THRESHOLD 25
#define PERIOD 1000
#define ITER_NUM 5

static const char* mq_name_temp = "/mq_temp";
static const char* I2C_DEV = "/dev/i2c-1";
static const int TEMP_CHANNEL = 0x01;

//메세지큐와 현재 읽은 값에 대한 변수
mqd_t mq_temp;
int curVal = 0;

//메세지큐, wiringPi 관련 함수
double getTemp(int value);
int getTempReg();
int initI2C();





int main(){
	int i2c_fd;

    char buf[BUFSIZE] = "0";
    int sum = 0;
    double temp[5] = {0};
	

	if(wiringPiSetupGpio() < 0 ){
		printf("wiringPiSetup() is failed\n");
		return 1;
	}

    i2c_fd = initI2C();
    mq_temp = mq_open(mq_name_temp, O_WRONLY);

    while(1){
        sum = 0;
        for(int i = 0; i < ITER_NUM; i++){
            int val = getTempReg(i2c_fd);
            temp[i] = getTemp(val);
            sum += temp[i];
            //printf("%d %0.2lf\n", val, temp[i]);
            delay(PERIOD);
        }
        if(temp[0] <= temp[4] && sum >= THRESHOLD*ITER_NUM){
            sprintf(buf, "%d", 1);
        }
        else{
            sprintf(buf, "%d", 0);
        }
        mq_send(mq_temp, buf, strlen(buf), 0);
    }

    mq_close(mq_temp);
	return 0;	
}



int initI2C(){
    int i2c_fd;
    if ((i2c_fd = wiringPiI2CSetupInterface (I2C_DEV, SLAVE_ADDR_01)) < 0 ){
		printf("wiringPi2CSetup Failed: \n");
		return 1;
	}
    return i2c_fd;
}
int getTempReg(int i2c_fd){
    int preVal = 0;
    int curVal = 0;
    
	wiringPiI2CWrite(i2c_fd, 0x40 | TEMP_CHANNEL);

	preVal= wiringPiI2CRead(i2c_fd);
	curVal = wiringPiI2CRead(i2c_fd);
    //1 변할때마다 11.7mV
    return curVal;
}
double getTemp(int value){
    return (100.0*(value*0.0129));
}

