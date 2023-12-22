#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

Servo myservo;  // Создаем объект myservo
bool cardActivated = false;  // Флаг для контроля активации карты

const int SS_PIN = 12;  // Пин подключения к SS (Slave Select)
const int RST_PIN = 5;  // Пин подключения к RST (Reset)

MFRC522 rfid(SS_PIN, RST_PIN);  // Создаем объект rfid

void dump_byte_array(byte *buffer, byte bufferSize);
bool checkCard(byte *uid);
void activateServo();

void setup() {
  Serial.begin(9600); // Инициализация последовательного порта
  SPI.begin();  // Инициализация SPI
  rfid.PCD_Init();  // Инициализация библиотеки MFRC522
  myservo.attach(3); // Подключаем сервопривод к пину 3
  myservo.write(-10); // Устанавливаем сервопривод в исходное положение
  Serial.println("Приложите карту RFID...");
}

// Определение функции checkCard
bool checkCard(byte *uid) {
    byte correctUid[] = {0x62, 0xD0, 0xAE, 0x89};

  // Проверяем каждый байт UID
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != correctUid[i]) {
      // Если хотя бы один байт не соответствует, возвращаем false
      return false;
    }
  }

  // Если все байты соответствуют, возвращаем true
  return true;
}

void loop() {
  // Проверяем, есть ли карта в области считывания
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.print("UID карты: ");
    dump_byte_array(rfid.uid.uidByte, rfid.uid.size);

    // Проверяем UID карты и выполняем код в зависимости от значения
    if (checkCard(rfid.uid.uidByte)) {
      Serial.println("МИША КРИНЖ, АХАХАХАХАХ");
      activateServo();
      cardActivated = true;  // Устанавливаем флаг в true, чтобы не активировать код
    } else {
      Serial.println("СПАСИБО, ДУРА");
    }

    // Завершаем считывание карты
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  // Проверяем условие для сброса флага активации карты
  if (cardActivated) {
    cardActivated = false;
  }
}

//Вывод массива байт в шестнадцатеричном формате 
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
  Serial.println();
}
//Функция активации сервопривода
void activateServo() {
  for (int pos = -10; pos <= 220; pos += 1) {
    myservo.write(pos);
    delay(15);
  }

  delay(1500);

  for (int pos = 220; pos >= 40; pos -= 1) {
    myservo.write(pos);
    delay(15);
  }
}