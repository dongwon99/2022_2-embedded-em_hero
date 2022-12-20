# 인덕션 화재 경보 시스템  
#### 2022-2 임베디드시스템 팀 임영웅  

<br/>

## 배경
요즘 가정에서는 가스레인지보다 인덕션의 사용량이 점점 증가하고 있는 추세이다.
2050세대 약 500명을 조사한 결과 77%가 인덕션을 사용할 의향이 있다고 밝힌 가운데 인덕션 화재 사건은 2013년도부터 꾸준히 늘어나고 있다.
편의에 맞게 바뀌어가는 주방 모습에 맞서 위험 요소를 제거할 필요성을 느꼈다.
  
<br/>

## 아이디어 소개
### 인덕션 화재 경보 시스템
사용자의 주거지 내 화재가 발생 위험이 있는 인덕션 옆에 설치하여 사용한다.
인덕션 위의 물체의 유무를 판단하는 초음파 센서와 인덕션 주변 온도를 측정하는 온도 센서를 활용하였다.
일정 온도 임계값에 도달한 상태에서 일정 거리 내 5초동안(False Alarm 방지) 물체가 없다고 인식되면 사용자가 인덕션을 끄지 않았다고 판단하고 비상등을 울리게 된다.
LED와 스피커로 구성된 비상등을 통해 사용자에게 위험 감지 신호를 준다.
만약 사용자가 비상등 알림을 20초 이내에 확인하고 스위치를 누른다면 화재가 아니라고 판단하고 1분 뒤에 다시 시스템을 반복 실행한다.
20초 이내에 스위치를 누르지 않는다면 소방서 및 사용자에게 화재 감지 알림 문자가 발송된다.

<br/>
  
## 개요
  - 목적: 인덕션 화재 감지   
  - 개발 환경: Linux
  - 사용 언어: c, python   
  - 사용 패키지 및 API:  
    - wiringPi: wiringPi.h, wiringPiI2C  
    - 주요 구현 방식
      - 멀티 프로세싱 및 IPC 사용
      - 라즈베리파이 - 스마트폰 통신 (coolsms 사용)

<br/>

## 전체 시스템 구조
### 시스템 구조

<div align="center">  
  <img width="650" alt="image" src="https://user-images.githubusercontent.com/84191029/208681389-34652283-d7a9-4a64-a95a-82fcabd1522f.png">
</div>

<br/>

  - 초음파 센서 및 온도 센서 → 장치 위에 물체의 유무 및 일정 온도 이상일 시 → LED 센서 및 스피커 센서 실행
  - LED 센서 및 스피커 센서 실행
    - 20초 전에 스위치 센서를 눌렀을 시 → 1분 후 반복 실행
    - 20초 이상 → 소방서에 연락

### 회로도

<div align="center">  
  <img width="450" alt="image" src="https://user-images.githubusercontent.com/84191029/208681431-a9dac7e5-177f-415e-b8cd-aa887112a6be.png">
</div>  

  
<br/>

## 코드 사용법 
###  main.c  
가장 기본이자 주요 코드인 main.c 파일.  
리눅스가 제공하는 spawn.h 파일의 posix_spawn 함수를 사용하여 물체의 거리와 온도를 수집한다.  
각 프로세스(온도, 거리)에서 특정 조건에 맞게 된다면 각각 메세지 큐로 1을 송신하게 된다.(아닐시 0)  
main 프로세스에서는 이 값을 검사하다가 온도와 거리 프로세스 둘 다 1을 송신한다면 수집을 그만두고 알람을 발생하게 된다.  

이 알람 또한 멀티 프로세싱, posix_spawn 함수를 통하여 알람을 발생한다. 알람은 led와 스피커가 이용된다.  
이 시점부터 시간을 잰다. 만약 사용자가 20초 안에 버튼을 누르지 못한다면 사용자의 폰, 그리고 119로 메세지가 가게 된다.  
20초 안에 버튼을 누른다면, 알람을 끄고 약 1분 정도 후 다시 시스템이 실행된다.  
  
  
### getCondition.c과 getDistance.c
이 2개의 소스코드는 구조가 아주 유사하다.  
먼저 main 프로세스와 통신하기 위해 메세지큐를 초기화하고, 5초간 총 5번 온도(거리)를 측정한다.  
만약 5초동안 측정한 결과가 특정 상황(계속해서 온도가 올라가거나, 앞에 물체가 5초동안 없거나)이라면  
메세지 큐로 1을 송신한다.  
  

### alarmSpk.c  
알람 프로세스이다. 2개의 핀(LED, 스피커)을 사용하고, 스피커는 주파수를 조정하기 위해 MarkSpace 모드를 사용하였다.  
20초 동안 알람이 울리고 20초가 지나면 프로세스가 종료된다.  

### send_messages.py  
사용자에게 문자를 보내기 위해 coolsms에서 지원하는 파이썬 sdk를 사용하였다.  
send_many 함수를 통해 사용자로부터 입력받은 주소값을 119에게, [알리미]비상! 비상! 인덕션을 끄지 않았습니다!! 라는 문구를 사용자에게 보낸다.  
그 이후 로그를 터미널에 찍게 된다.  
자세한 내용은 아래 coolsms 웹사이트를 참조.  

https://docs.coolsms.co.kr/development-kits/python

<br/>

## 앱 사용영상   
<div align="center">  
  

[![Thumbnail](https://user-images.githubusercontent.com/102962030/208643627-03eaa442-31a6-4056-89ab-0782b8970a39.png)](https://youtu.be/N2bZ5JN9U3Y)    

</div>  
 
<br/>

## 기여자   
 
<div align="center">
  <a href="https://github.com/dongwon99">
    <img src="https://user-images.githubusercontent.com/102962030/206461747-4d56a152-6963-46ea-853a-603465037070.png", width=200, alt="dongwon", title="dongwon99"/>
  </a>
  <a href="https://github.com/withsme">
    <img src="https://user-images.githubusercontent.com/102962030/208648196-1ac2df02-3a15-4651-b544-5d75b1e40089.jpg", width=200, alt="seung mi", title="seung mi"/>
  </a>
  <a href="https://github.com/yelim1215">
    <img src="https://user-images.githubusercontent.com/102962030/206461754-a786f9e2-953c-4a9f-971e-62c00ffe2a49.png", width=200, alt="yelim1215", title="yelim1215"/>
  </a>  
</div>  

<br/>

## 참고문헌 
http://www.gasnews.com/news/articleView.html?idxno=74052
https://digitalchosun.dizzo.com/site/data/html_dir/2022/08/05/2022080580098.html
