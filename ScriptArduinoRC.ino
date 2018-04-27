#include <SoftwareSerial.h>
#include "Arduino.h"
#include <stdio.h>

SoftwareSerial BT(10, 11);

/*
  ピン番号 3 : モータ出力 左タイヤ
  ピン番号 5 : モータ出力 右タイヤ
  ピン番号 6 : HIGH 左タイヤ前進
  ピン番号 7 : HIGH 左タイヤ後進
  ピン番号 8 : HIGH 右タイヤ前進
  ピン番号 9 : HIGH 右タイヤ後進
*/

#define MOTOR_POWER_LEFT 3
#define MOTOR_POWER_RIGHT 5
#define MOTOR_FRONT_LEFT 6
#define MOTOR_BACK_LEFT 7
#define MOTOR_FRONT_RIGHT 8
#define MOTOR_BACK_RIGHT 9

void setup() {
  BT.begin(9600);
  Serial.begin(38400);

  //左タイヤ
  pinMode(MOTOR_FRONT_LEFT, OUTPUT);
  pinMode(MOTOR_BACK_LEFT, OUTPUT);

  //右タイヤ
  pinMode(MOTOR_FRONT_RIGHT, OUTPUT);
  pinMode(MOTOR_BACK_RIGHT, OUTPUT);

}

/*-------------------------------------------------
 * コントローラーから送られてくるデータフォーマット
 * imageId : ラジコンに対する動作命令
 *    1 : 前進  2 : 後退  3 : 左回転  4 : 右回転
 * Time : 動作命令の実行時間
 * Speed : 動作命令のモータパワー値
 * 
 * 例: 102100 (1命令分) (6byte)
 * 
 * 1桁目 -> imageId
 * 2,3桁目 -> Time
 * 4,5,6桁目 -> Speed
 *
 * モータ値100で2秒間前進
-------------------------------------------------*/

void loop() {
  //Serial.available()で取得できるのは最大64byteなので、現状は一度に10命令分までしか実行できない
  if (BT.available() > 0) { 

    String data = BT.readStringUntil('\0'); 
    Serial.print(data+"\n");
    checkData(data);

    sleepMotor();
  }

}

void checkData(String data){
  
  //imageIdの取り出し(1桁)
  String imageCode = data.substring(0, 1);
  //Serial.print(imageCode+"\n");

  //Timeの取り出し(2桁)
  int timeCode = data.substring(1, 3).toInt();
  //Serial.print(String(timeCode)+"\n");

  //speedの取り出し 右回転(3桁)
  int rightSpeedCode = data.substring(3, 6).toInt();
  //Serial.print(String(rightSpeedCode)+"\n");

  //speedの取り出し 左回転(3桁)
  int leftSpeedCode = data.substring(6, 9).toInt();
  //Serial.print(String(leftSpeedCode)+"\n");

  motorControl(imageCode, timeCode, rightSpeedCode, leftSpeedCode);
  
}

//TODO モーター出力値の調整
void motorControl(String image, int timeNum, int rightSpeed, int leftSpeed) {

  timeNum = timeNum * 1000; //単位msに変換

  //前進
  if (image.equals("1")) {
    //左タイヤ
    digitalWrite(MOTOR_FRONT_LEFT, HIGH);
    digitalWrite(MOTOR_BACK_LEFT, LOW);

    //右タイヤ
    digitalWrite(MOTOR_FRONT_RIGHT, HIGH);
    digitalWrite(MOTOR_BACK_RIGHT, LOW);

    //出力調整
    analogWrite(MOTOR_POWER_LEFT, leftSpeed);
    analogWrite(MOTOR_POWER_RIGHT, rightSpeed);
    
    delay(timeNum);
    
  }

  //後退
  else if (image.equals("2")) {
    //左タイヤ
    digitalWrite(MOTOR_FRONT_LEFT, LOW);
    digitalWrite(MOTOR_BACK_LEFT, HIGH);

    //右タイヤ
    digitalWrite(MOTOR_FRONT_RIGHT, LOW);
    digitalWrite(MOTOR_BACK_RIGHT, HIGH);

    //出力調整
    analogWrite(MOTOR_POWER_LEFT, leftSpeed);
    analogWrite(MOTOR_POWER_RIGHT, rightSpeed);

    delay(timeNum);
  }

  //左回転
  else if (image.equals("3")) {
    //左タイヤ
    digitalWrite(MOTOR_FRONT_LEFT, LOW);
    digitalWrite(MOTOR_BACK_LEFT, HIGH);

    //右タイヤ
    digitalWrite(MOTOR_FRONT_RIGHT, HIGH);
    digitalWrite(MOTOR_BACK_RIGHT, LOW);

    //出力調整
    analogWrite(MOTOR_POWER_LEFT, leftSpeed);
    analogWrite(MOTOR_POWER_RIGHT, rightSpeed);

    delay(timeNum);
  }

  //右回転
  else if (image.equals("4")) {
    //左タイヤ
    digitalWrite(MOTOR_FRONT_LEFT, HIGH);
    digitalWrite(MOTOR_BACK_LEFT, LOW);

    //右タイヤ
    digitalWrite(MOTOR_FRONT_RIGHT, LOW);
    digitalWrite(MOTOR_BACK_RIGHT, HIGH);

    //出力調整
    analogWrite(MOTOR_POWER_LEFT, leftSpeed);
    analogWrite(MOTOR_POWER_RIGHT, rightSpeed);

    delay(timeNum);
  }

  //停止
  else if(image.equals("5")){
    //左タイヤ
    digitalWrite(MOTOR_FRONT_LEFT, LOW);
    digitalWrite(MOTOR_BACK_LEFT, LOW);

    //右タイヤ
    digitalWrite(MOTOR_FRONT_RIGHT, LOW);
    digitalWrite(MOTOR_BACK_RIGHT, LOW);

    delay(timeNum);
  }

}

void sleepMotor(){
  digitalWrite(MOTOR_FRONT_LEFT, LOW);
  digitalWrite(MOTOR_BACK_LEFT, LOW);
  digitalWrite(MOTOR_FRONT_RIGHT, LOW);
  digitalWrite(MOTOR_BACK_RIGHT, LOW);

  delay(100);
}


