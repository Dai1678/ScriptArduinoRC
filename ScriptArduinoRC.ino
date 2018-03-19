#include <SoftwareSerial.h>
#include "Arduino.h"
#include <stdio.h>

SoftwareSerial BT(10, 11);

/*
  ピン番号 6 : HIGH 左タイヤ前進
  ピン番号 7 : HIGH 左タイヤ後進
  ピン番号 8 : HIGH 右タイヤ前進
  ピン番号 9 : HIGH 右タイヤ後進
*/

void setup() {
  BT.begin(9600);
  Serial.begin(38400);

  //左タイヤ
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  //右タイヤ
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

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

  //speedの取り出し(3桁)
  int speedCode = data.substring(3, 6).toInt();
  //Serial.print(String(speedCode)+"\n");

  motorControl(imageCode, timeCode, speedCode);
  
}

//TODO モーター出力値の調整
void motorControl(String image, int timeNum, int speedNum) {

  timeNum = timeNum * 1000; //単位msに変換

  //前進
  if (image.equals("1")) {
    //左タイヤ
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);

    //右タイヤ
    digitalWrite(8, HIGH);
    digitalWrite(9, LOW);

    //出力調整
    analogWrite(3, speedNum);
    analogWrite(5, speedNum);
    
    delay(timeNum);
    
  }

  //後退
  else if (image.equals("2")) {
    //左タイヤ
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);

    //右タイヤ
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);

    //出力調整
    analogWrite(3, speedNum);
    analogWrite(5, speedNum);

    delay(timeNum);
  }

  //左回転
  else if (image.equals("3")) {
    //左タイヤ
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);

    //右タイヤ
    digitalWrite(8, HIGH);
    digitalWrite(9, LOW);

    //出力調整
    analogWrite(3, speedNum);
    analogWrite(5, speedNum);

    delay(timeNum);
  }

  //右回転
  else if (image.equals("4")) {
    //左タイヤ
    digitalWrite(6, HIGH);
    digitalWrite(7, LOW);

    //右タイヤ
    digitalWrite(8, LOW);
    digitalWrite(9, HIGH);

    //出力調整
    analogWrite(3, speedNum);
    analogWrite(5, speedNum);

    delay(timeNum);
  }

  //停止
  else if(image.equals("5")){
    //左タイヤ
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);

    //右タイヤ
    digitalWrite(8, LOW);
    digitalWrite(9, LOW);

    delay(timeNum);
  }

}

void sleepMotor(){
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);

  delay(100);
}


