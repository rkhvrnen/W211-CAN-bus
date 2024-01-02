#include "ic_display.h"

#define REQUEST_ID 0x07e0
#define RESPONSE_ID 0x07e8

#define AUDIO_PAGE 0x03
#define TEL_PAGE 0x05
#define LEFT 0x00
#define CENTER 0x10

#define B 'B'
#define C 'C'

void setup() {
  Serial.begin(115200);
  SPI.begin();

  initCanB();
  initCanC();

  initDisplay();
  delay(300);
  writeHeader(AUDIO_PAGE, CENTER, "Turbo:");
  delay(200);
  Serial.println("Start.");
}

int request_interval = 200;
unsigned long lastRequest = 0;

int update_interval = 200;
unsigned long lastUpdate = 0;

int value = 0;
float pressure = 0.00;

void loop() {
  //Send KWP2000 -request for MAP
  if(millis() - lastRequest >= request_interval){
    writeCan(C, REQUEST_ID, 8, 0x02, 0x01, 0x0b, 0xff, 0xff, 0xff, 0xff, 0xff, false);
    //Serial.println("KWP2000-request sent");
    lastRequest = millis();
  }

  //Update display text
  if(millis() - lastUpdate >= update_interval){
    char disp[20];
    char num[5];
    if(value <= 100){pressure = 0.00;}else{pressure = ((float(value) - 100) / 100);}
    dtostrf(pressure, 4, 2, num);
    sprintf(disp, "%s Bar", num);
    writeText(AUDIO_PAGE, disp);
    //Serial.println("Screen updated");
    lastUpdate = millis();
  }

  readCanMsg(C, RESPONSE_ID, value);
  Serial.println(value);

}