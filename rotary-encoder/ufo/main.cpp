#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define RS_ENABLE PORTA |= (1<<PINA0);
#define RS_DISABLE PORTA &= ~(1<<PINA0);
#define EN_ENABLE PORTA |= (1<<PINA1);
#define EN_DISABLE PORTA &= ~(1<<PINA1);
#define LCD_PORT PORTA
#define SPEAKER_PORT PINA2

void beep(void);
void lcd_write_str(char *str);
void init_display(void);
void lcd_write_nibble(int data);
void lcd_cmd(int data);
void clear_display(void);

int z = 0;
//char str[4];
int main(void){ 
//     Port A initialization
//     Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out
    DDRA=(1<<DDA7) | (1<<DDA6) | (1<<DDA5) | (1<<DDA4) | (1<<DDA3) | (1<<DDA2) | (1<<DDA1) | (1<<DDA0);
    PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);
//     Port D initialization
//     Function: Bit4=In Bit3=In Bit2=In
    DDRD=(0<<DDD2) | (0<<DDD3) | (0<<DDD4);
    PORTD=(1<<PORTD2) | (1<<PORTD3) | (1<<PORTD4);
//     External Interrupt(s) initialization
//     INT0: On
//     INT0 Mode: Low level
//     INT1: on
//     INT1 Mode: Rising Edge
//     INT2: Off
    GICR=(1<<INT1) | (1<<INT0);
    MCUCR=(1<<ISC11) | (1<<ISC10) | (0<<ISC01) | (0<<ISC00);

    init_display();
    lcd_write_str("Hello");
    sei();
    
    while (1){
		//to do
    }
}

// External Interrupt
// Encoder
ISR(INT0_vect) {
    char str[4];
    if((((PIND) & (1<<PD2)) == 0) ^ (((PIND) & (1<<PD4)) == 0))
    {
        z++;
    }else{
        z--;
    }
    clear_display();
    sprintf(str, "%d", z);
    lcd_write_str(str);
    _delay_ms(30);
}

// External Interrupt
// Button
ISR(INT1_vect){
    beep();
}

void beep(void) {
    int a;
    for(a=0; a<100; a++) {
         PORTA ^= 1 << SPEAKER_PORT;
         _delay_us(100);
    }
	PORTA &= ~1<<PINA2;
}
/***************************************************************************************************
*   Send string to LCD
****************************************************************************************************/
 void lcd_write_str(char *str) {
    do {
        lcd_cmd(*str);
    }while(*++str);
}

/***************************************************************************************************
*   Init LCD
****************************************************************************************************/
void init_display(void) {
    RS_DISABLE; //rs = 0
    _delay_ms(1000);
    
    lcd_write_nibble(0x30);
    _delay_ms(50);  
//              
    lcd_write_nibble(0x30);
    _delay_ms(50);
//
    lcd_write_nibble(0x20);
    _delay_ms(50);
//        
    lcd_cmd(0x28);   //set data length 4 bit 2 line
    _delay_ms(50);    
//
    lcd_cmd(0x0E);   
    _delay_ms(50);
//
    lcd_cmd(0x01);   //clear display
    _delay_ms(50);
//	        
    RS_ENABLE;  //rs = 1   
}
/***************************************************************************************************
*  8bit command LCD
****************************************************************************************************/
void lcd_write_nibble(int data) {
    PORTA |= data;
    EN_ENABLE;
    _delay_ms(2);
    EN_DISABLE;  
    PORTA &= ~(data);   
} 

/***************************************************************************************************
*   4bit command LCD
****************************************************************************************************/
void lcd_cmd(int data) {
    lcd_write_nibble(data & 0xF0); //H bytes
    lcd_write_nibble(data << 4); //L bytes
}
/***************************************************************************************************
*   Clear LCD
****************************************************************************************************/
void clear_display(void) {
     RS_DISABLE;
     lcd_cmd(0x01); 
     _delay_ms(5); 
     RS_ENABLE;
}

