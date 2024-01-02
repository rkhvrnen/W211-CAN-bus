#include "can_communication.h"

#define CAN_B CAN_83K3BPS
#define CAN_C CAN_500KBPS

#define CAN_B_Pin 10
#define CAN_C_Pin 8

MCP2515 mcp2515_b(CAN_B_Pin);
MCP2515 mcp2515_c(CAN_C_Pin);

void initCanB(){
  mcp2515_b.reset();
  mcp2515_b.setBitrate(CAN_B);
  mcp2515_b.setNormalMode();
}

void initCanC(){
  mcp2515_c.reset();
  mcp2515_c.setBitrate(CAN_C, MCP_8MHZ);
  mcp2515_c.setNormalMode();
}

void writeCan(char bus, uint16_t PID, int DLC, uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6, uint8_t data7, bool printing){
  struct can_frame frame;
  frame.can_id = PID;
  frame.can_dlc = DLC;
  frame.data[0] = data0;
  frame.data[1] = data1;
  frame.data[2] = data2;
  frame.data[3] = data3;
  frame.data[4] = data4;
  frame.data[5] = data5;
  frame.data[6] = data6;
  frame.data[7] = data7;
  if(bus == 'B'){mcp2515_b.sendMessage(&frame);}
  else if (bus =='C'){mcp2515_c.sendMessage(&frame);}
  else(Serial.println("Error, bus does not exist."));
  if(printing){
    Serial.print("CAN Message sent to CAN-bus "); Serial.print(bus); Serial.print(", ");
    Serial.print("ID: "); Serial.print(PID, HEX);
    Serial.print(" Data: ");
    print(frame.data[0]); Serial.print(":");
    print(frame.data[1]); Serial.print(":");
    print(frame.data[2]); Serial.print(":");
    print(frame.data[3]); Serial.print(":");
    print(frame.data[4]); Serial.print(":"); 
    print(frame.data[5]); Serial.print(":");
    print(frame.data[6]); Serial.print(":");
    print(frame.data[7]); Serial.println();
  }
}

void print(uint8_t X) {
   if (X < 16) {Serial.print("0");}
   Serial.print(X, HEX);
}

void readCanMsg(char bus, uint16_t PID, int& variable){
  if(bus == 'C'){
    struct can_frame canMsg;
    if (mcp2515_c.readMessage(&canMsg) == MCP2515::ERROR_OK) {
        char buffer[7];
        if(canMsg.can_id == PID){
          //Serial.print("RESPONSE GET. FRAME:ID="); Serial.print(canMsg.can_id); Serial.print(":LEN="); Serial.print(canMsg.can_dlc);
          char tmp[3];
          for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
            char buffer[5];
            //Serial.print(":");
            sprintf(buffer,"%02X", canMsg.data[i]);
            //Serial.print(buffer);
          }
          //Serial.println();
          variable = canMsg.data[3];
          //Serial.print("Value: "); Serial.print(value); Serial.println();
        }

    }
  }
}