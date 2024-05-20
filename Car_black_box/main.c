/*
 * File:   main.c
 */

#include "main.h"

#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)




void init_config(void) {
    init_i2c(100000); //100K  initialize i2c
    init_ds1307();    //initialize RTC
    
    init_clcd();     //initialize clcd
    init_digital_keypad(); //initialize digitalkeypad
    init_adc();      //initialize ADC
    
    init_timer2();   //initialization of timers for login screen to wait for 15min when login is blocked
    GIE=1;
    PEIE=1;
}

void main(void) {
    
    unsigned char reset_flag; //01 for default screen 
    unsigned char control_flag=DASH_BOARD_FLAG;
    char event[]="ON";
    unsigned char speed =0,key; //
    char *gear[]={"GN","G1","G2","G3","G4","GR"};
    unsigned char gr=0;
     char  menu_pos =0;
    int wait_time = 0;
    
    static unsigned char select=0,long_up_press=0;
    init_config();
    
    log_event(event,speed);
    
    eeprom_at24c04_str_write(0x00,"1010");

    while (1) {
       
        speed=read_adc()/10;
        if(speed > 99)
        {
            speed=99;
        }
        
        key=read_digital_keypad(LEVEL);
        for(unsigned int wait=0;wait==5000;wait++);//delay for avoiding bouncing effect
        
        if(key == SW1)
        {
            strcpy(event,"C ");
        }
        else if( key == SW2 && gr < 6)
        {
            strcpy(event,gear[gr]);
            gr++;
            if(gr==6) 
                gr=5;
        }
        else if(key== SW3 && gr > 0)
        {
            gr--;
            
            if(gr==0)
                gr=0;
            strcpy(event,gear[gr]);
        }
        else if((key == SW4 || key == SW5) && control_flag == DASH_BOARD_FLAG)
        {
            control_flag = LOGIN_FLAG ;
            clear_screen();
            clcd_print("ENTER PASSWORD",LINE1(1));
            clcd_write(LINE2(7),INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON,INST_MODE);
            //clcd_print("    ",LINE2(0));
            
            reset_flag=RESET_PASSWORD;
            
            TMR2ON=1;
        
        }
         else if ( key == LPSW4 && control_flag == LOGIN_MENU_FLAG )
		{
            

			switch ( menu_pos )
			{
				case 0:                              //to view log
					clcd_write(0x01,INST_MODE);
					clcd_print("Logs",LINE1(0));
					control_flag = VIEW_LOG_FLAG;
					reset_flag = RESET_VIEW_LOG_POS ;
					break;
				case 1 :                              //to clear log
					clcd_write(0x01,INST_MODE);
					control_flag = CLEAR_LOG_FLAG ;
					reset_flag = RESET_MEMORY;
					break;
				case 2 :                              //download log
					log_event("DL",speed);
					download_log();
                 clcd_write(0x01,INST_MODE);
					control_flag = LOGIN_MENU_FLAG;
					reset_flag = RESET_LOGIN_MENU;
					break;
				case 3 :                               // set time
					log_event("CT",speed);
					clcd_write(0x01,INST_MODE);
					control_flag = SET_TIME_FLAG;
					reset_flag = RESET_TIME;
					break;
				case 4:                                  //change password
					log_event("CP",speed);
					clcd_write(0x01,INST_MODE);
					control_flag = CHANGE_PASSWORD_FLAG;
					reset_flag = RESET_PASSWORD;
					DISP_ON_AND_CURSOR_ON;
					TMR2ON = 1;
					break;

			}
        
		}
         else if ( key == LPSW5 && (control_flag == LOGIN_MENU_FLAG ||control_flag == VIEW_LOG_FLAG))
            {
                control_flag = DASH_BOARD_FLAG;
                clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            }
        else if ( key == LPSW4 && control_flag == VIEW_LOG_FLAG )
            {
                control_flag = LOGIN_MENU_FLAG;
                clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
            }
        
        switch(control_flag)
        {
            case DASH_BOARD_FLAG:
            {
                 
                 display_dash_board(event,speed);
                 break;
            
            }
            case LOGIN_FLAG:
            {
                
                __delay_ms(100);
               
               switch(login(key,reset_flag))
               {    
                   case RETURN_BACK :
                   {
                    control_flag=DASH_BOARD_FLAG; 
                    TMR2ON=0;
                    clcd_write(DISP_ON_AND_CURSOR_OFF,INST_MODE);
                    clear_screen();
                    break;
                   }
                   case LOGIN_SUCCESS:
                   {
                    control_flag=LOGIN_MENU_FLAG;
                    reset_flag=RESET_LOGIN_MENU;
                    continue;
                    break;
                   }
               }
               break;
            
            }
            case LOGIN_MENU_FLAG:
            {
                 select=menu(reset_flag,key);  
                 break;   
            }
            case VIEW_LOG_FLAG:
            {
                view_log(key,reset_flag);
                break;
            }
            case CLEAR_LOG_FLAG : 
                //clear logs
				if ( clear_log(reset_flag) == TASK_SUCCESS )
					wait_time = 50;
                
				if ( reset_flag == RESET_MEMORY )
					log_event("CL",speed);
                
				if (wait_time > 0 && --wait_time == 0 )
				{
					control_flag = LOGIN_MENU_FLAG;
					reset_flag = RESET_LOGIN_MENU;
                   clcd_write(0x01,INST_MODE);
					continue;
				}
                
				break;
                
            case SET_TIME_FLAG : //set time
                
				if ( change_time( key ,reset_flag ) == TASK_SUCCESS )
					wait_time = 100;
                
				if (wait_time > 0 && --wait_time == 0 )
				{
					control_flag = LOGIN_MENU_FLAG;
					reset_flag = RESET_LOGIN_MENU;
                    clcd_write(0x01,INST_MODE);
					continue;
				}
				break;
            
            
        
         }
        
          reset_flag=RESET_NOTHING;
    }
    
    return;
}
