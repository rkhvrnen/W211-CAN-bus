#include "ic_display.h"

#define AGW_PID 0x01a4
#define B 'B'

void initDisplay(){
  //Package 20
  delay(2000);
  writeCan(B, AGW_PID, 8, 0x05, 0x03, 0x20, 0x02, 0x11, 0xc3, 0xd4, 0x07, true);
  delay(1785);

  //ACK
  writeCan(B, AGW_PID, 8, 0x03, 0x03, 0x21, 0x06, 0x11, 0xc3, 0xd4, 0x07, true);
  delay(50);

  //Package 24
  writeCan(B, AGW_PID, 8, 0x10, 0x13, 0x03, 0x24, 0x02, 0x00, 0x01, 0x01, true);
  delay(37);
  writeCan(B, AGW_PID, 8, 0x21, 0x00, 0x00, 0x00, 0x12, 0x00, 0x01, 0x00, true);
  delay(25);
  writeCan(B, AGW_PID, 8, 0x22, 0x02, 0x10, 0x20, 0x20, 0x00, 0xd6, 0x00, true);
  delay(80);

  //Package 25
  writeCan(B, AGW_PID, 8, 0x03, 0x03, 0x25, 0x06, 0x40, 0x01, 0x01, 0x00, true);
  delay(100);

  //Package 26
  writeCan(B, AGW_PID, 8, 0x10, 0x0a, 0x03, 0x26, 0x01, 0x00, 0x01, 0x03, true);
  delay(47);
  writeCan(B, AGW_PID, 8, 0x21, 0x10, 0x00, 0x00, 0x9d, 0x01, 0x00, 0x00, true);
  delay(100);
}

void writeHeader(int screen, int justification, char * input){
  uint8_t data[14];
  uint8_t length = strlen(input);

  if(length > 8){Serial.println("Too long header."); return;}

  if(length < 4){data[0] = 0x09;} else{data[0] = length + 5;}
  data[1] = screen; data[2] = 0x29; data[3] = justification;

  for(int i = 0; i < length; i++){
    data[4 + i] = input[i];
  }

  if(length <= 3){
    for(int i = (length + 4); i < 8; i++){
      data[i] = 0x20;
    }
    length = 4;
  }
  data[0] = length + 5;
  data[4 + length] = 0x00;
  data[5 + length] = calcCSC2(data, length);

  //Send to CANBUS
  writeCan(B, AGW_PID, 8, 0x10, data[0], data[1], data[2], data[3], data[4], data[5], data[6], true);
  delay(47);
  writeCan(B, AGW_PID, 8, 0x21, data[7], data[8], data[9], data[10], data[11], data[12], data[13], true);
}

//Package 26, body text
void writeText(int screen, char * input){
  uint8_t length = strlen(input);
  uint8_t csb = length + 3;
  uint8_t csa = csb + 7;
  uint8_t csc = calcCSC(screen, input);
  writeCan(B, AGW_PID, 8, 0x10, csa, screen, 0x26, 0x01, 0x00, 0x01, csb, false);
  delay(47);

  if(length <= 3){
    uint8_t data[6];
    for(int j = 0; j < length; j++){
      data[j] = input[j];
    }
    data[length] = 0x00;
    data[length + 1] = 0x00;
    data[length + 2] = csc;
    for(int j = length + 3; j < 6; j++){
      data[j] = 0x00;
    }
    writeCan(B, AGW_PID, 8, 0x21, 0x10, data[0], data[1], data[2], data[3], data[4], data[5], false);
  }

  else if(length >= 4 && length <= 10){
    uint8_t data[13];
    for(int j = 0; j < length; j++){
      data[j] = input[j];
    }
    data[length] = 0x00;
    data[length + 1] = 0x00;
    data[length + 2] = csc;
    for(int j = length + 3; j < 13; j++){
      data[j] = 0x00;
    }

    writeCan(B, AGW_PID, 8, 0x21, 0x10, data[0], data[1], data[2], data[3], data[4], data[5], false);
    delay(47);
    writeCan(B, AGW_PID, 8, 0x22, data[6], data[7], data[8], data[9], data[10], data[11], data[12], false);
  }

  else if(length >= 11 && length <= 17){
    uint8_t data[20];
    for(int j = 0; j < length; j++){
      data[j] = input[j];
    }
    data[length] = 0x00;
    data[length + 1] = 0x00;
    data[length + 2] = csc;
    for(int j = length + 3; j < 20; j++){
      data[j] = 0x00;
    }

    writeCan(B, AGW_PID, 8, 0x21, 0x10, data[0], data[1], data[2], data[3], data[4], data[5], false);
    delay(47);
    writeCan(B, AGW_PID, 8, 0x22, data[6], data[7], data[8], data[9], data[10], data[11], data[12], false);
    delay(45);
    writeCan(B, AGW_PID, 8, 0x23, data[13], data[14], data[15], data[16], data[17], data[18], data[19], false);
  }

  else{
    Serial.println("TO BE IMPLEMENTED");
  }
}

int calcCSC(int screen, char * input){
  //Serial.println(input);
  int len = strlen(input);
  uint8_t csb = len + 3;
  int sum_pos = 1 + 2 + 3 + 4 + 5 + 6; //7 initial bytes in the RAW
  int sum_raw = screen + 0x26 + 1 + 1 + csb + 0x10; //Initial raw sum from 03 26 01 00 01 09 10

  //Calculate the sum of position indices and the sum of raw stream
  for(int i = 0; i < len; i++){
    sum_pos += (7 + i);
    sum_raw += input[i];
  }
  sum_pos += (6 + len) * 2 + 3; //Two 0x00 bytes at the end --> pos values added

  int mod1 = 255 - (sum_pos + sum_raw);
  int csc = mod1 % 256;

  //Serial.print("POS: "); Serial.println(sum_pos);
  //Serial.print("RAW: "); Serial.println(sum_raw);
  //Serial.print("CSC: "); Serial.println(csc);

  if(csc < 0){return csc + 256;}
  else{return csc;}
}

int calcCSC2(uint8_t *data, int length){
  int sum_pos = 0; int sum_raw = 0;

  for(int j = 0; j <= (length + 3); j++){
    sum_pos += j;
  }

  for(int j = 1; j <= (length + 5); j++){
    sum_raw += data[j];
  }

  int mod1 = 255 - (sum_pos + sum_raw);
  int csc = mod1 % 256;
  //Serial.println(); Serial.print("POS: "); Serial.print(sum_pos); Serial.print(" RAW: "); Serial.print(sum_raw);
  //Serial.print(" Mod1: "); Serial.print(mod1); Serial.print(" CSC: "); Serial.print(csc); Serial.println();
  if(csc < 0){return csc + 256;}else{return csc;}
}