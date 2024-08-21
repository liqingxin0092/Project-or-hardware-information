#ifndef __LCD_H
#define __LCD_H

void WriteData(uint8_t data);
void WriteCommand(uint8_t command);
void WriteString(char *ptr);
uint8_t ReadBusy(void);

#endif
