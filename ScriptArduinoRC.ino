#include <SoftwareSerial.h>

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
 * 例: 102100 (1命令分)
 * 1桁目 -> imageId
 * 2,3桁目 -> Time
 * 4,5,6桁目 -> Speed
 *
 * モータ値100で2秒間前進
-------------------------------------------------*/

void loop() {
  int btDataSize = BT.available();
  if (btDataSize > 0) {
    char btData[btDataSize];

    for(int i=0; i<btDataSize; i++){
      btData[i] = BT.read();
      Serial.print(btData[i]);
    }
    
    //normalization(btData);
  }

}

void normalization(String select) {
  int i, j;
  int scriptLength = select.length() / 3;
  String imageCode[scriptLength];
  int timeNumCode[scriptLength];
  int speedNumCode[scriptLength];

  //imageIdを表す数字の取り出し(1桁)
  for (i = 0, j = 0; i < select.length() - 6; i += 6, j++) {
    imageCode[j] = select.charAt(i);
  }

  //Timeを表す数字の取り出し(2桁)
  for (i = 1, j = 0; i < select.length() - 5; i += 6, j++) {
    String timeStrCode = select.substring(i, i + 2);
    timeNumCode[j] = timeStrCode.toInt();
  }

  //speedを表す数字の取り出し(3桁)
  for (i = 3, j = 0; i < select.length() - 3; i += 6, j++) {
    String speedStrCode = select.substring(i, i + 3);
    speedNumCode[j] = speedStrCode.toInt();
  }

  //
  for (int i = 0; i < scriptLength; i++) {
    motorControl(imageCode[i], timeNumCode[i], speedNumCode[i]);
  }

}

void motorControl(String image, int timeNum, int speedNum) {

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

}

