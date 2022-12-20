
all: main getCondition getDistance alarmSpk reset

main:main.o
	gcc -o main main.o -lrt -lwiringPi
main.o:main.c
	gcc -c main.c

getCondition:getCondition.o
	gcc -o getCondition getCondition.o -lrt -lwiringPi
getCondition.o:getCondition.c
	gcc -c getCondition.c

getDistance:getDistance.o
	gcc -o getDistance getDistance.o -lrt -lwiringPi
getDistance.o:getDistance.c
	gcc -c getDistance.c


alarmSpk:alarmSpk.o
	gcc -o alarmSpk alarmSpk.o -lwiringPi
alarmSpk.o:alarmSpk.c
	gcc -c alarmSpk.c

reset:reset.o
	gcc -o reset reset.o -lwiringPi
reset.o:reset.c
	gcc -c reset.c



clean:
	rm -f main.o
	rm -f getCondition.o
	rm -f getDistance.o
