/* 
 * File:   uart.h
 * Author: Karthik
 *
 * Created on 25 March, 2024, 1:17 PM
 */

#ifndef UART_H
#define	UART_H

#define FOSC                20000000

void init_uart(unsigned long baud);
unsigned char getcharx(void);
void putchars(unsigned char data);
void putsx(const char *s);
//void getsx(char *string);






#endif	/* UART_H */

