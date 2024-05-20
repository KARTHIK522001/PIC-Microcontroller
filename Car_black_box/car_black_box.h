/* 
 * File:   car_black_box.h
 * Author: Karthik
 *
 * Created on 7 March, 2024, 5:12 PM
 */

#ifndef CAR_BLACK_BOX_H
#define	CAR_BLACK_BOX_H

void display_dash_board(char event[],unsigned char speed);

void log_event(char event[],unsigned char speed);

void log_car_event(void);

void clear_screen(void);

unsigned char login(unsigned char key,unsigned char reset_flag);

unsigned char menu(unsigned char reset_flag,unsigned char key);

unsigned char view_log(unsigned char key,char reset_shift);

char clear_log (char reset_memory);

void download_log(void);

char change_time( unsigned char key ,unsigned char reset_time );

#endif	/* CAR_BLACK_BOX_H */

