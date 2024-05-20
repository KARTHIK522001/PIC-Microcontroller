/* 
 * File:   at24co4.h
 * Author: Karthik
 *
 * Created on 9 March, 2024, 2:48 PM
 */

#ifndef AT24CO4_H
#define	AT24CO4_H

#define SLAVE_WRITE_EE     0xA0    //1010A2A1P00    
#define SLAVE_READ_EE      0xA1   //1010A2A1P01



unsigned char eeprom_at24c04_read(unsigned char memory_loc);
void eeprom_at24c04_byte_write(unsigned char memory_loc,unsigned char data);
void eeprom_at24c04_str_write(unsigned char memory_loc,char *data);
#endif	/* AT24CO4_H */

