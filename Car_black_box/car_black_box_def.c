/*Function definition*/
#include "main.h"


unsigned char clock_reg[3]; // HH MM SS
                               // 0   1  2 

 char time[7]; 
 
 char log[11];   // to store hh mm ss e speed
 
 int pos = -1;
 
 int sec=60,min;
 
 int return_time=5;
 
 int access = -1;
 
 char overflow = 0;
 
 char *menu_list[]={"View log","Clear log","Download log","Set time","Change Passwrd","               "};
 

void get_time()
{
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 
    
    
    //BCD Format 
    //clock_reg[0] = HH
    //clock_reg[1] = MM
    //clock_reg[2] = SS
   
    /* To store the time in HH:MM:SS format */
    // "HH:MM:SS"
    // HH -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    
    
    // MM 
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    
   
    // SS
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
    
    
    
}

void display_time()
{
     get_time();
    
    clcd_putch(time[0], LINE2(0)); // HH:MM:SS 
    clcd_putch(time[1], LINE2(1));
    clcd_putch(':', LINE2(2));
    clcd_putch(time[2], LINE2(3));
    clcd_putch(time[3], LINE2(4));
    clcd_putch(':', LINE2(5));
    clcd_putch(time[4], LINE2(6));
    clcd_putch(time[5], LINE2(7));
    
}



void display_dash_board(char event[],unsigned char speed)
{
      clcd_print("  TIME   E SPEED", LINE1(0));
      // get_time(clock_reg); // HH MM SS
      display_time();  
      // display_time(clock_reg); // HH:MM:SS --> 13:14:15
      clcd_print(event, LINE2(9));
      clcd_putch(speed/10+'0',LINE2(13));
      
      clcd_putch(speed%10+'0',LINE2(14));
}



void log_event(char event[],unsigned char speed)
{
    get_time();
    strncpy(log,time,6);
    strncpy(log,event,2);
    log[8]=speed/10 +'0';
    log[8]=speed%10 +'0';
    log[9]='\0';
    log_car_event();
}

void log_car_event(void)
{

    char addr;
    addr=5;
    pos++;
    if(pos == 10)
    {
        pos=0;
    }
    addr = (pos*10)+ 5;  //pos=5 15 25 35
    eeprom_at24c04_str_write(addr,log); //0x05 0x15 0x25 0x35


}

void clear_screen(void )
{

    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);

}


unsigned char login(unsigned char key,unsigned char reset_flag)
{

    static char usepass[5],i;
    static unsigned  char attempt;
    char spass[4];
    
    if(reset_flag == RESET_PASSWORD)
    {
        attempt=3;
        usepass[0]='\0';
        usepass[1]='\0';
        usepass[2]='\0';
        usepass[3]='\0';
        usepass[4]='\0';
        i=0;
        key=ALL_RELEASED;
        return_time=5;
        
    }
    if(return_time == 0)
    {
        return  RETURN_BACK;
    }
    if( key == SW4 && i < 4 )
    {
        usepass[i]='1';
        i++;
        clcd_putch('*',LINE2(6+i));
        return_time=5;
    }
    else if( key == SW5 && i < 4 )
    {
        usepass[i]='0';
        i++;
        clcd_putch('*',LINE2(6+i));
        return_time=5;
    }
    if(i == 4)
    {
        for(int j=0;j<4;j++)
        {
        
           spass[j] =eeprom_at24c04_read(j);
        }
        if(strncmp(usepass,spass,4) == 0)
        {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF,INST_MODE);
            clcd_print("Login SUCCESS",LINE1(1));
            __delay_ms(2000);
            return LOGIN_SUCCESS;
        }
        else
        {
            attempt--;
            clear_screen();
            if(attempt==0)
            {
               clcd_write(DISP_ON_AND_CURSOR_OFF,INST_MODE);
               clcd_print("You are blocked",LINE1(0));
               attempt=3;
               sec=60;
               min=3;
               while(min)
               {
                  clcd_putch(min/10+'0',LINE2(1));
                  clcd_putch(min%10+'0',LINE2(2));
                  clcd_putch(':',LINE2(3));
                  clcd_putch(sec/10+'0',LINE2(4));
                  clcd_putch(sec%10+'0',LINE2(5));
                  clcd_print("Min Left",LINE2(7));
               }
               min=3;
            
            }
            else
            {
                clcd_write(DISP_ON_AND_CURSOR_OFF,INST_MODE);
                clcd_print("Wrong Password",LINE1(1));
                clcd_putch(attempt+'0',LINE2(1));
                clcd_print("attempts left",LINE2(3));
               __delay_ms(2000);
            }
            clear_screen();
            clcd_print("ENTER PASSWORD",LINE1(1));
            clcd_write(LINE2(7),INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON,INST_MODE);
            return_time=5;
            i=0;
        }
    
    }
    
}    


unsigned char menu(unsigned char reset_flag,unsigned char key)
 {
         static int menu_pos = 0  ;
	static int select_pos = 1 ;
	if ( reset_flag == RESET_LOGIN_MENU )
	{
		menu_pos = 0;
		select_pos = 1;
	}
    /*logic for scrolling menu and star*/
	if ( key == SW4 && menu_pos < 4 ) //sw4 to forward
	{
		menu_pos++;
		if ( select_pos < 2 )
			select_pos++;
        
        clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
	}
    
	else if( key == SW5 && menu_pos > 0) //sw5 to move back
	{
		menu_pos--;
		if ( select_pos > 1 )
			select_pos--;
        clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
	}

	if ( select_pos == 1 )
	{
		clcd_putch('*',LINE1(0));
		clcd_print( menu_list[menu_pos],LINE1(2));
		clcd_print( menu_list[menu_pos+1],LINE2(2));
	}
	else
	{
		clcd_putch('*',LINE2(0));
		clcd_print( menu_list[menu_pos-1],LINE1(2));
		clcd_print( menu_list[menu_pos],LINE2(2));
	}

	return menu_pos;
}
    
    
unsigned char view_log(unsigned char key,char reset_shift)
{

    static int shift ;
	char rlog[11];
	rlog[10]= 0;
	int rpos ;
	unsigned char add;
	if ( access == -1 )
	{
		clcd_print ("No logs available" , LINE2(0));
	}
	else
	{
		if ( reset_shift == RESET_VIEW_LOG_POS )
			shift = 0 ;
		rpos = (overflow)? (pos + 1) % 10 : 0;

		if ( key == SW4 && shift < access ) //sw4 to move forward
        {
			shift++;
            clcd_print("                 ", LINE2(0)) ;
        }
		else if ( key == SW5 && shift > 0 )// sw5 to move backward
        {
			shift--;
            clcd_print("                 ", LINE2(0)) ;
        }

		rpos = (rpos + shift) % 10;

		for ( int i = 0 ; i < 10 ; i++ )
		{
			add = rpos * 10 + 5 + i ;
			rlog[i] = eeprom_at24c04_read(add);
		}
		/*printing logs on clcd one by one*/
		clcd_putch ( shift + '0' , LINE2(0));
		clcd_putch ( rlog[0] , LINE2(2));
		clcd_putch ( rlog[1] , LINE2(3));
		clcd_putch ( ':' , LINE2(4));
		clcd_putch ( rlog[2] , LINE2(5));
		clcd_putch ( rlog[3] , LINE2(6));
		clcd_putch ( ':' , LINE2(7));
		clcd_putch ( rlog[4] , LINE2(8));
		clcd_putch ( rlog[5] , LINE2(9));
		clcd_putch ( rlog[6] , LINE2(11));
		clcd_putch ( rlog[7] , LINE2(12));
		clcd_putch ( rlog[8] , LINE2(14));
		clcd_putch ( rlog[9] , LINE2(15));
        for(unsigned long int i =0;i < 20000;i++);
        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
	}



}


char clear_log (char reset_memory)
{
	clcd_print( "Logs cleared",LINE1(2));
	clcd_print( "successfully", LINE2(2));
    /*clearing all the logs*/
	if ( reset_memory == RESET_MEMORY )
	{
		pos = -1;
		access = -1 ;
		return TASK_SUCCESS;
	}
	return TASK_FAIL;
}

void download_log(void)
{
	int next = -1 ;
	char rlog[11];
	rlog[10] = 0;
	int rpos = 0 ;
	unsigned char add;
    //display if no logs available
	if ( access == -1 )
	{
		putsx("No logs available");
	}
	else
	{
        //displaying all logs available
		putsx("Logs :");
		putchars('\n');
		putchars('\n');
		putchars('\r');

		while (  next < access )
		{
			next++;
			rpos = (overflow)? (pos + 1) % 10 : 0;
			rpos = (rpos + next) % 10;

			for ( int i = 0 ; i < 10 ; i++ )
			{
				add = rpos * 10 + 5 + i ;
				rlog[i] = eeprom_at24c04_read(add);
			}
            //printing all logs in cutecom
			putchars ( next + '0');
			putchars(' ');
			putchars ( rlog[0] );
			putchars ( rlog[1] );
			putchars (':');
			putchars ( rlog[2] );
			putchars ( rlog[3] );
			putchars (':');
			putchars ( rlog[4] );
			putchars ( rlog[5] );
			putchars (' ');
			putchars ( rlog[6] );
			putchars ( rlog[7] );
			putchars (' ');
			putchars ( rlog[8] );
			putchars ( rlog[9] );
			putchars ( '\n' );
			putchars ('\r');
		}
	}
}


char change_time( unsigned char key ,unsigned char reset_time )
{
    /*logic to change time*/
	static unsigned int new_time[3];
	static int blink_pos ;
	static unsigned char wait , blink ;
	char dummy;
	static char t_done = 0;
	if ( reset_time == RESET_TIME )
	{
		get_time();
		new_time[0] = (time[0] & 0x0F ) * 10 + (time[1] & 0x0F) ;
		new_time[1] = (time[2]  & 0x0F ) * 10 + (time[3] & 0x0F) ;
		new_time[2] = (time[4] & 0x0F ) * 10 + (time[5] & 0x0F) ;
		clcd_print("Time (HH:MM:SS)",LINE1(0));
		blink_pos = 2;
		wait = 0;
		blink = 0;
		key = ALL_RELEASED;
		t_done = 0;
	}

	if( t_done)
		return TASK_FAIL;

	switch ( key )
	{

		case SW4 :
			new_time[blink_pos]++;
			break;
		case SW5 :
			blink_pos = (blink_pos + 1) % 3;
			break;
		case SW6 :
            /*storing new time*/
			get_time();
			dummy = ((new_time[0] / 10 ) << 4 ) | new_time[0] % 10 ;
			clock_reg[0] = (clock_reg[0] & 0xc0) | dummy ;
			write_ds1307(HOUR_ADDR, clock_reg[0] );
			dummy = ((new_time[1] / 10 ) << 4 ) | new_time[1] % 10 ;
			clock_reg[1] = (clock_reg[1] & 0x80) | dummy;
			write_ds1307(MIN_ADDR, clock_reg[1] );
			dummy = ((new_time[2] / 10 ) << 4 ) | new_time[2] % 10 ;
			clock_reg[2] = (clock_reg[2] & 0x80) | dummy;
			write_ds1307(SEC_ADDR, clock_reg[2] );
			clcd_write(0x01,INST_MODE);
			clcd_print("Time changed",LINE1(2));
			clcd_print("Successfully",LINE2(2));
            for(unsigned long int i = 0;i < 20000;i++);
			t_done = 1;
			return  TASK_SUCCESS;
	}

	if( new_time[0]  > 23 )
		new_time[0] = 0;
	if ( new_time[1] > 59 )
		new_time[1] = 0;
	if ( new_time[2] > 59 )
		new_time[2] = 0;

	if ( wait ++ == 50 )
	{
		wait = 0;
		blink = !blink;
		clcd_putch( new_time[0] / 10 + 48, LINE2(0));
		clcd_putch( new_time[0] % 10 + 48 , LINE2(1));
		clcd_putch( ':', LINE2(2));
		clcd_putch( new_time[1] / 10 + 48, LINE2(3));
		clcd_putch( new_time[1] % 10 + 48, LINE2(4));
		clcd_putch( ':', LINE2(5));
		clcd_putch( new_time[2] / 10 + 48 , LINE2(6));
		clcd_putch( new_time[2] % 10 + 48, LINE2(7));
/*logic to blink at the current pos*/

		if ( blink)
		{
			switch( blink_pos )
			{
				case 0:
					clcd_print("  ",LINE2(0));
					break;
				case 1:
					clcd_print("  ",LINE2(3));
					break;
				case 2:
					clcd_print("  ",LINE2(6));
					break;
			}
		}
	}
	return TASK_FAIL;
}
