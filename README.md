# 인덕션 화재 경보 시스템  
#### 2022-2 임베디드시스템 팀 임영웅  
해당 시스템은 사용자가 요리를 마치고 인덕션을 끄지 않을 때를 대비하여 제작한 시스템입니다.  
False Alarm을 방지하기 위하여 5초동안 초음파, 온도 센서로 상황을 인식합니다.  
5초 동안 인덕션 위에 냄비 등의 물체가 존재하지 않고, 온도가 계속해서 올라가거나 유지된다면 알람을 울리게 됩니다.  

  
  
  
## 앱 사용영상   

<div align="center">  
  

[![Thumbnail](https://user-images.githubusercontent.com/102962030/208643627-03eaa442-31a6-4056-89ab-0782b8970a39.png)](https://youtu.be/N2bZ5JN9U3Y)    

</div>  
  
  
## 개요
  - 목적: 인덕션 화재 감지   
  - 개발 환경: Linux
  - 사용 언어: c, python   
  - 사용 패키지 및 API:  
    - wiringPi: wiringPi.h, wiringPiI2C  
    - Login: Kakao, Naver  
    - Map: Google Map, Places API, Geocoding API  


  - 서버: 카카오, 네이버 로그인 서버와 통신하기 위한 node.js 서버 구축  
  - 서버 코드: https://github.com/2022-opensourse-mobile/2022-opensource-mobile-server
  
  - DB : Firebase(NoSQL)  
  - DB 구조 :  

![img](https://firebasestorage.googleapis.com/v0/b/a-living-dictionary.appspot.com/o/DB%20Structure.png?alt=media&token=4c21c918-3ffd-4009-b60d-35b2a8689876)




  
  
  
  
  
  
  
## 코드 사용법  
### 디렉토리 구조
![img](https://firebasestorage.googleapis.com/v0/b/a-living-dictionary.appspot.com/o/DIR%20Structure.png?alt=media&token=d7d01223-3d56-401f-b2d5-b8f928309317)
### 설명
###   - main.c  
가장 기본이자 주요 코드인 main.c 파일.  
리눅스가 제공하는 spawn.h 파일의 posix_spawn 함수를 사용하여 물체의 거리와 온도를 수집한다.  
각 프로세스(온도, 거리)에서 특정 조건에 맞게 된다면 각각 메세지 큐로 1을 송신하게 된다.(아닐시 0)  
main 프로세스에서는 이 값을 검사하다가 온도와 거리 프로세스 둘 다 1을 송신한다면 수집을 그만두고 알람을 발생하게 된다.  

이 알람 또한 멀티 프로세싱, posix_spawn 함수를 통하여 알람을 발생한다. 알람은 led와 스피커가 이용된다.  
이 시점부터 시간을 잰다. 만약 사용자가 20초 안에 버튼을 누르지 못한다면 사용자의 폰, 그리고 119로 메세지가 가게 된다.  
20초 안에 버튼을 누른다면, 알람을 끄고 약 1분 정도 후 다시 시스템이 실행된다.  
  
  
###   - getCondition.c과 getDistance.c
이 2개의 소스코드는 구조가 아주 유사하다.  
먼저 main 프로세스와 통신하기 위해 메세지큐를 초기화하고, 5초간 총 5번 온도(거리)를 측정한다.  
만약 5초동안 측정한 결과가 특정 상황(계속해서 온도가 올라가거나, 앞에 물체가 5초동안 없거나)이라면  
메세지 큐로 1을 송신한다.  
  

###   - alarmSpk.c  
알람 프로세스이다. 2개의 핀(LED, 스피커)을 사용하고, 스피커는 주파수를 조정하기 위해 MarkSpace 모드를 사용하였다.  
20초 동안 알람이 울리고 20초가 지나면 프로세스가 종료된다.  

###   - send_messages.py  
사용자에게 문자를 보내기 위해 coolsms에서 지원하는 파이썬 sdk를 사용하였다.  
send_many 함수를 통해 사용자로부터 입력받은 주소값을 119에게, [알리미]비상! 비상! 인덕션을 끄지 않았습니다!! 라는 문구를 사용자에게 보낸다.  
그 이후 로그를 터미널에 찍게 된다.  
자세한 내용은 아래 coolsms 웹사이트를 참조.  

https://docs.coolsms.co.kr/development-kits/python

 
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
