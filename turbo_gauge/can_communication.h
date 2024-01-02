#include "mcp2515.h"

void initCanB();
void initCanC();
void print(uint8_t X);
void writeCan(char bus, uint16_t PID, int DLC, uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6, uint8_t data7, bool printing);
void readCanMsg(char bus, uint16_t PID, int &value);