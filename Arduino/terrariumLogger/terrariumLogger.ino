
#include <SdFat.h>
#include <Wire.h>
#include <RTClib.h>

SdFat sd;
SdFile myFile;

RTC_DS1307 rtc;

const int chipSelect = 10;

void writeLog(int temperature) {
  DateTime now = rtc.now();

  char outputData [50] = "";

  sprintf(outputData, "%04d/%02d/%02d %02d:%02d:%02d, %04d, %04d, %04d, %04d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second(), temperature, temperature, temperature, temperature);

  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) sd.initErrorHalt();

  if (!myFile.open("terrarium.csv", O_RDWR | O_CREAT | O_AT_END)) {
    sd.errorHalt("Opening terrarium.csv for write failed");
  }

  myFile.println(outputData);

  myFile.close();
}

void setup() {
  if (! rtc.begin()) {
    sd.errorHalt("Couldn't find RTC");
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  //writeLog(1);
}
