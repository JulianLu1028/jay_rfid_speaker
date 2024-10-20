#include <SPI.h>
#include "MFRC522.h"
#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// RFID 設定
#define SS_PIN 5    // SDA 連接到 GPIO5 (D1)
#define RST_PIN 22  // RST 連接到 GPIO22
MFRC522 rfid(SS_PIN, RST_PIN);  // 初始化 MFRC522 物件

// DFPlayer Mini 設定
HardwareSerial mySerial(1);  // 使用硬件串口 1（UART1）
DFRobotDFPlayerMini myDFPlayer;

// 音樂對應的 RFID UID
String uid1 = "44CE17DC";  // 假設第一張卡的 UID
String uid2 = "C41E21DC";  // 假設第二張卡的 UID

void setup() {
  // 初始化串口，硬件串口 1 與 DFPlayer Mini 進行通信
  mySerial.begin(9600, SERIAL_8N1, 16, 17);  // UART1: RX = GPIO16, TX = GPIO17
  Serial.begin(115200);  // USB 串口，用於調試
  
  // 初始化 DFPlayer Mini
  if (!myDFPlayer.begin(mySerial)) {  
    Serial.println("DFPlayer Mini 初始化失敗！");
    while (true);
  }
  myDFPlayer.volume(20);  // 設置音量

  // 初始化 RFID
  SPI.begin();  // SPI 通信
  rfid.PCD_Init();  // 初始化 MFRC522

  Serial.println("RFID 和 DFPlayer Mini 已初始化");
}

void loop() {
  // 檢查是否有 RFID 卡
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // 檢查是否讀取卡片
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // 獲取 UID
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);  // 將 UID 轉換為字串格式
  }
  uid.toUpperCase();  // 轉換為大寫

  Serial.print("檢測到的卡 UID: ");
  Serial.println(uid);

  // 根據 UID 播放對應音樂
  if (uid == uid1) {
    Serial.println("播放音樂 1");
    myDFPlayer.stop();  // 停止當前音樂
    myDFPlayer.play(1);  // 播放第一首音樂
  } else if (uid == uid2) {
    Serial.println("播放音樂 2");
    myDFPlayer.stop();  // 停止當前音樂
    myDFPlayer.play(2);  // 播放第二首音樂
  } else {
    Serial.println("未識別的卡，播放預設音樂");
    myDFPlayer.stop();  // 停止當前音樂
    myDFPlayer.play(3);  // 播放預設音樂
  }

  // 等待卡片移走
  rfid.PICC_HaltA();
}
