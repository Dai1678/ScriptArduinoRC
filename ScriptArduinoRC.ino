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
  // put your main code here, to run repeatedly:

}
