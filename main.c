#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h> 
#include <mqueue.h>
#include <time.h>
#include <string.h>
#include <wiringPi.h>
#include <spawn.h>
#include <signal.h>
#include <stdlib.h>

#define BUFSIZE 32
#define SWITCH 24


const char* mq_name_temp = "/mq_temp";
const char* mq_name_dis = "/mq_dis";


mqd_t mq_temp, mq_dis;
struct mq_attr attr;

void initAttr();
void initMQ();
void closeMQ();
int getValue(char* buf);
void exitGetFunc(pid_t pid_temp, pid_t pid_dis);


int main(){
    //메세지 큐로부터 값을 읽기 위한 버퍼
    char buf_temp[BUFSIZE], buf_dis[BUFSIZE], buf_stop[BUFSIZE];
    //메세지 큐로부터 읽은 값을 INT로 저장
    int detect_temp, detect_dis;
    //멀티 프로세싱에 필요한 매개변수
    char *argv_temp[] = {"getCondition", NULL};
    char *argv_dis[] = {"getDistance", NULL};
    char *argv_spk[] = {"alarmSpk", NULL};
    char *argv_re[] = {"reset", NULL};
    //멀티 프로세싱 프로세스ID
    pid_t pid_spk, pid_temp, pid_dis, pid_reset;
    //버튼 값
    int read = 0;
    //타이머
    clock_t begin, end;


    wiringPiSetupGpio();
    pinMode(SWITCH, INPUT);
    initAttr();

    char addr[60];
    printf("주소 : ");
    scanf("%s", addr);
    printf("%s\n", addr);


    while(1){
        //초기 변수 설정
        initMQ();
        detect_temp = 0;
        detect_dis = 0;

        //온도 측정 프로세스와 물체 거리 측정 프로세스
        posix_spawn(&pid_temp, "./getCondition", NULL, NULL, argv_temp, NULL);
        posix_spawn(&pid_dis, "./getDistance", NULL, NULL, argv_dis, NULL);

        //센서가 보내주는 메세지 감시
        while(!detect_temp || !detect_dis){
            if(mq_receive(mq_dis, buf_dis, sizeof(buf_dis), NULL) != -1){
                detect_dis = getValue(buf_dis);
            }
            if(mq_receive(mq_temp, buf_temp, sizeof(buf_temp), NULL) != -1){
                detect_temp = getValue(buf_temp);
            }

            printf("sensor(temp,dist) : %d, %d\n", detect_temp, detect_dis);
            memset(buf_dis, 0, sizeof(buf_dis));
            memset(buf_temp, 0, sizeof(buf_temp));
        }
        //while 탈출 : 비상 상황 발생

        //센서 끄고 알람 발생
        printf("run alarm\n");
        exitGetFunc(pid_temp, pid_dis);
        posix_spawn(&pid_spk, "./alarmSpk", NULL, NULL, argv_spk, NULL);

        //버튼 입력 시작 및 시작 시간 설정
        begin = clock();
        while(1){
            read = digitalRead(SWITCH);
            end = clock();
            if(read == 1){
                printf("stop\n");
                break;
            }
            if(((end - begin) / CLOCKS_PER_SEC) > 20.0){
                //sms 전송
                char msg[100];
                sprintf(msg, "python3 send_messages.py %s", addr);
                system(msg);
                break;
            }
        }
        //버튼을 입력하거나 20초가 지났을 경우(알람 종료)
        kill(pid_spk, SIGINT);
        posix_spawn(&pid_reset, "./reset", NULL, NULL, argv_re, NULL);
        waitpid(pid_reset, NULL, 0);

        closeMQ();
        delay(60000);
    }//다시 무한반복
}


void initAttr(){
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = BUFSIZE;
    attr.mq_curmsgs = 0;
}
void initMQ(){
    mq_temp = mq_open(mq_name_temp, O_RDONLY | O_CREAT, 0777, &attr);
    mq_dis = mq_open(mq_name_dis, O_RDONLY | O_CREAT, 0777, &attr);
}
void closeMQ(){
    mq_close(mq_temp);
    mq_close(mq_dis);
    mq_unlink(mq_name_temp);
    mq_unlink(mq_name_dis);
}
int getValue(char* buf){
    switch(buf[0]){
        case '1':
        return 1;
    default:
        return 0;
    }
}
void exitGetFunc(pid_t pid_temp, pid_t pid_dis){
    kill(pid_temp, 9);
    kill(pid_dis, 9);
    closeMQ();
}