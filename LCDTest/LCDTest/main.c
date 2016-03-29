/*
 * LCDTest.c
 *
 * Created: 3/29/2016 12:39:01 AM
 * Author : speku
 */ 

#include <avr/io.h>
#include <lcd.h>

int main(void)
{
    lcd_init(LCD_DISP_ON);
	lcd_puts("Hello World");
    while (1) 
    {
    }
}

