#ifndef UART_H
#define UART_H

void UartXmitChar(char  c);
void UartXmitString(char * str);

void UartInit(void);
#define UART_BAUDRATE_9600		104
#define UART_BAUDRATE_19200	 	52
#define UART_BAUDRATE_38400		26
#define UART_BAUDRATE_57600		8




#endif // UART_H
