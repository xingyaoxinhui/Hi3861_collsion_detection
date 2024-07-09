#ifndef __SERIAL_H
#define __SERIAL_H
    void Serial_Init(void);
	void Serial_SendByte(uint8_t Byte);
	void Serial_SendArray(uint8_t *Array, uint16_t Length);
	void Serial_SendString(char *string);
	
#endif
