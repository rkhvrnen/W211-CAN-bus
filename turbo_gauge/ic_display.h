#include <stdint.h>
#include <string.h>
#include "can_communication.h"

void initDisplay();
void writeText(int screen, char *input);
void writeHeader(int screen, int justification, char *input);
int calcCSC(int screen, char *input);
int calcCSC2(uint8_t *data, int length);