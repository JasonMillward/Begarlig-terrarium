
#include <SdFat.h>
#include <Wire.h>
#include <RTClib.h>
#include <dht.h>
#include <MQ135.h>

dht DHT;
SdFat sd;
SdFile outFile;

MQ135 gasSensor = MQ135(A2);

RTC_DS1307 rtc;

const int chipSelect = 10;
const int LDRPin = A0;
const int DHT22_PIN = 7;

char outputData [120] = "";

void writeLog(int temperature, int humidity, int LDRvalue, int oxygenPPM) {
  DateTime now = rtc.now();

  sprintf(outputData, "%04d/%02d/%02d %02d:%02d:%02d, %8d, %8d, %8d, %8d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), temperature, humidity, LDRvalue, oxygenPPM);

  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  if (!outFile.open("terr.csv", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("Opening terrarium.csv for write failed");
  }

  Serial.println(outputData);
  outFile.println(outputData);

  outFile.close();
}

void writeErr(String errorMsg) {

  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  if (!outFile.open("error.log", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("Opening error.log for write failed");
  }

  outFile.println(errorMsg);
  outFile.close();
}

int getLDRValue() {
  return analogRead(LDRPin);
}

void refreshDHTSensor() {

  uint32_t start = micros();
  int chk = DHT.read22(DHT22_PIN);
  uint32_t stop = micros();

  switch (chk) {
    case DHTLIB_OK:
      break;
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

float getO2PPM() {
  return gasSensor.getPPM();
}

float rzero() {
  return gasSensor.getRZero();
}

void setup() {
  Serial.begin(115200);

  if (! rtc.begin()) {
    writeErr("Couldn't find RTC");
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

}

void loop() {
  delay(2000);

  refreshDHTSensor();

  writeLog( (int) getTempValue(), (int) getHumidValue(), getLDRValue(), (int) getO2PPM());
}
