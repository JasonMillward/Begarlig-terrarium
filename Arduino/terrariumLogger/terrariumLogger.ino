
#include <SdFat.h>
#include <Wire.h>
#include <RTClib.h>
#include <dht.h>


dht DHT;
SdFat sd;
SdFile myFile;

RTC_DS1307 rtc;

const int chipSelect = 10;
const int LDRPin = A0;
const int DHT22_PIN = 7;

void writeLog(float temperature, float humidity, int LDRvalue, int oxygen) {
  DateTime now = rtc.now();

  char outputData [50] = "";
  char t[7];
  char h[7];

  dtostrf(temperature, 2, 2, t);
  dtostrf(humidity, 2, 2, h);

  sprintf(outputData, "%04d/%02d/%02d %02d:%02d:%02d, %04s, %04s, %04d, %04d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), t, h, LDRvalue, temperature, oxygen);

  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  if (!myFile.open("terr.csv", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("Opening terrarium.csv for write failed");
  }

  myFile.println(outputData);

  myFile.close();
}

void writeErr(String errorMsg) {

  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  if (!myFile.open("error.log", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("Opening error.log for write failed");
  }

  myFile.println(errorMsg);
  myFile.close();
}

int getLDRValue() {
  return analogRead(LDRPin);
}

void refreshDHTSensor() {

  uint32_t start = micros();
  int chk = DHT.read22(DHT22_PIN);
  uint32_t stop = micros();

  switch (chk) {
    case DHTLIB_ERROR_CHECKSUM:
      writeErr("Checksum error");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      writeErr("Time out error");
      break;
    case DHTLIB_ERROR_CONNECT:
      writeErr("Connect error");
      break;
    case DHTLIB_ERROR_ACK_L:
      writeErr("Ack Low error");
      break;
    case DHTLIB_ERROR_ACK_H:
      writeErr("Ack High error");
      break;
    default:
      writeErr("Unknown error");
      break;
  }
}

float getTempValue() {
  return DHT.temperature;
}

float getHumidValue() {
  return DHT.humidity;
}

void setup() {
  if (! rtc.begin()) {
    writeErr("Couldn't find RTC");
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

}

void loop() {
  delay(10000);

  refreshDHTSensor();

  writeLog( getTempValue(), getHumidValue(), getLDRValue(), 0 );
}
