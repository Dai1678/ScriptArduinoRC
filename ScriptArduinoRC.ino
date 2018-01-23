#include <SoftwareSerial.h>

SoftwareSerial BT(10,11);

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
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);

  //右タイヤ
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);

}

void loop() {
  String btData;
  if(Serial.available() > 0){
    btData = BT.read();
    Serial.print(btData);
    normalization(btData);
  }
  
}

void normalization(String select){
  int i,j;
  int scriptLength = select.length() / 3;
  String actionCode[scriptLength];
  int timeNumCode[scriptLength];
  int speedNumCode[scriptLength];
  
  //actionを表す数字の取り出し
  for(i=0,j=0; i<select.length() - 6; i+=6,j++){
    actionCode[j] = select.charAt(i);
  }

  //Timeを表す数字の取り出し
  for(i=1,j=0; i<select.length() - 5; i+=6,j++){
    String timeStrCode = select.substring(i,i+2);
    timeNumCode[j] = timeStrCode.toInt();
  }

  for(i=3,j=0; i<select.length() - 3; i+=6,j++){
    String speedStrCode = select.substring(i,i+3);
    speedNumCode[j] = speedStrCode.toInt();
  }

  for(int i=0; i<scriptLength; i++){
    motorControl(actionCode[i],timeNumCode[i],speedNumCode[i]);
  }

}

void motorControl(String action, int timeNum, int speedNum){

  //前進
  if (action.equals("1")){
    //左タイヤ
    digitalWrite(6,HIGH);
    digitalWrite(7,LOW);

    //右タイヤ
    digitalWrite(8,HIGH);
    digitalWrite(9,LOW);

    //出力調整
    analogWrite(3,speedNum);
    analogWrite(5,speedNum);

    delay(timeNum);

  }

  //後退
  else if (action.equals("2")){
    //左タイヤ
    digitalWrite(6,LOW);
    digitalWrite(7,HIGH);

    //右タイヤ
    digitalWrite(8,LOW);
    digitalWrite(9,HIGH);

    //出力調整
    analogWrite(3,speedNum);
    analogWrite(5,speedNum);

    delay(timeNum);
  }
  
  //左回転
  else if (action.equals("3")){
    //左タイヤ
    digitalWrite(6,LOW);
    digitalWrite(7,HIGH);

    //右タイヤ
    digitalWrite(8,HIGH);
    digitalWrite(9,LOW);

    //出力調整
    analogWrite(3,speedNum);
    analogWrite(5,speedNum);

    delay(timeNum);
  }

  //右回転
  else if (action.equals("4")){
    //左タイヤ
    digitalWrite(6,HIGH);
    digitalWrite(7,LOW);

    //右タイヤ
    digitalWrite(8,LOW);
    digitalWrite(9,HIGH);

    //出力調整
    analogWrite(3,speedNum);
    analogWrite(5,speedNum);

    delay(timeNum);
  }

}

