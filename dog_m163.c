//****************************************************  //
//														//
//
//  "Treiber" für EA-DOGM 163 im Hardware-SPI Mode
//    getestet am Atmega8
//
//                                          Tino Kühn
//         tino (punkt) kuehn (ät) gmaail (punkt) com
//****************************************************


#include "meinboard.h" // generelles file für alle includes
// gebraucht wird #include <util/delay.h>


// Anschluss des Displays gemäß der Vorgabe aus dem Datenblatt für SPI (Seite 4)
// Die Ports RS und CSB können beliebig am Controler gewählt werden.
// Die Ports SI und CLK müssen an den Pins für's SPI des Controlers angeschlossen
// werden. SI < - > MOSI (an PDIP Atmega8 PB3) und CLK < - > SCK (an PDIP Atmega8 PB5)

// Vorgehen:
// 1) SPI initialisieren (SS muss auch als Ausgang definiert sein, sonst klappt Hardware SPI nicht)
// 2) Display initalisieren
// 3) Display Befehle oder Daten schicken

//Datenblätter:
// http://www.lcd-module.de/eng/pdf/zubehoer/st7036.pdf
// http://www.lcd-module.de/pdf/doma/dog-m.pdf
// http://www.uni-koblenz.de/~physik/informatik/MCU/SPI.pdf

// Anmerkung: die Dokumentation ist nicht so schön gestaltet aber mit Glück schafft man es trotzdem...


#define DOGM163_CSB 	PB1 	// Port fuer CSB am µC
#define DOGM163_RS 		PB0  	// Port für RS am µC

#define DOGM163_DDR 	DDRB	// DDR  am Atemega8
#define DOGM163_PORT 	PORTB	// Port am Atemega8


#define CHARACTER_BUFFER_BASE_ADDRESS	0x80  	//Konstante die zum Setzen der Spalte und Zeile gebraucht wird
#define CHARACTERS_PER_ROW 				16 		//Anzahl der Spalten am Display



void init_PORTS_DOGM163 (void)
{
	//RS und CSB als Ausgang setzen
	DOGM163_DDR |= (1<<DOGM163_RS) | (1<<DOGM163_CSB);
}


// Das Display resettet den Counter bei der fallenden Flanke vom CSB 
void reset_CSB_DOGM163 (void)
{
	DOGM163_PORT |= (1<<DOGM163_CSB);
	DOGM163_PORT &= ~(1<<DOGM163_CSB);
}


void write_instruction_DOGM163 (char instruction) 
{
	//RS low = instruction
	DOGM163_PORT &= ~(1 << DOGM163_RS);
	//resetten des Counters
	reset_CSB_DOGM163();
	// instruktionen senden
	spi_transmit(instruction);
	_delay_ms(2);
	//Display wieder deaktivieren (chip select=1)
	DOGM163_PORT |= (1<<DOGM163_CSB);
}


void write_char_DOGM163 (char instruction) 
{
	//RS high = data
	DOGM163_PORT |= (1 << DOGM163_RS);
	//resetten des Counters
	reset_CSB_DOGM163();
	// Daten senden
	spi_transmit(instruction);
	_delay_ms(2);
	//Display wieder deaktivieren (chip select=1)
	DOGM163_PORT |= (1<<DOGM163_CSB);
}


void write_string_DOGM163 (char *buffer) // sollte selbsterklärend sein
{
	while (*buffer) 
	{
    write_char_DOGM163(*buffer);
    buffer++;
    }
}




void init_3lines_DOGM163(void) 
{
	init_PORTS_DOGM163();
	_delay_ms(200);
	write_instruction_DOGM163 (0x38); // Instruction Set, {0 0 1 DL N DH IS2 IS1} = 00 11 10 00 
	write_instruction_DOGM163 (0x39); // Instruction Set, {0 0 1 DL N DH IS2 IS1} = 00 11 10 01 
	write_instruction_DOGM163 (0x1D); // Bias (Spannung), {0 0 0 1 BS 1 0 Fx} = 00010101, es kann 15/1D gewählt werden, dann leuchtet der Hintergrund stark nach
	write_instruction_DOGM163 (0x7F); // Contrast (Bit C3-C0) {0 1 1 1 C3 C2 C1 C0} = 01111111
	write_instruction_DOGM163 (0x5F); // Power/ICON/Contrast (Bit C5-C4) {0 1 0 1 Ion Bon C5 C4} = 01011111
	write_instruction_DOGM163 (0x69); // Follower {0 1 1 0 Fon Rab2 Rab1 Rab0} = 01101001, Rab siehe Datenblatt Power supply
    _delay_ms(200);
	write_instruction_DOGM163 (0x0C); // Display Function, {0 0 0 0 1 D C B} = 00001100
	write_instruction_DOGM163 (0x06); // Entry Mode {0 0 0 0 0 1 I/D S} = 00000110
}


void init_3lines_DOGM163_v2(void)  // für das mit weißem Backlight
{
	init_PORTS_DOGM163();
	_delay_ms(200);
	write_instruction_DOGM163 (0x39); // Instruction Set, {0 0 1 DL N DH IS2 IS1} = 00 11 11 10  
	write_instruction_DOGM163 (0x15); // Bias (Spannung), {0 0 0 1 BS 1 0 Fx} = 00010101, es kann auch 1D gewählt werden, dann leuchtet der Hintergrund stark nach
	write_instruction_DOGM163 (0x54); // Power/ICON/Contrast (Bit C5-C4) {0 1 0 1 Ion Bon C5 C4} = 01011111
	write_instruction_DOGM163 (0x6E); // Follower {0 1 1 0 Fon Rab2 Rab1 Rab0} = 01101001, Rab siehe Datenblatt Power supply
	write_instruction_DOGM163 (0x7C); // Contrast (Bit C3-C0) {0 1 1 1 C3 C2 C1 C0} = 01111111
	write_instruction_DOGM163 (0x38); // Instruction Set, {0 0 1 DL N DH IS2 IS1} = 00 11 10 00 
	_delay_ms(200);
	write_instruction_DOGM163 (0x0C); // Display Function, {0 0 0 0 1 D C B} = 00001100
	write_instruction_DOGM163 (0x06); // Entry Mode {0 0 0 0 0 1 I/D S} = 00000110
}
/*

	init_PORTS_DOGM163();
	_delay_ms(200);
	write_instruction_DOGM163 (0x3E); // Instruction Set, {0 0 1 DL N DH IS2 IS1} = 00 11 11 10  
	write_instruction_DOGM163 (0x18); // Double hight position {0 0 0 1 DH x x x } 00 01 0x xx
	write_instruction_DOGM163 (0x3D); // Instruction Set, {0 0 1 DL N DH IS2 IS1} = 00 11 11 10  
	write_instruction_DOGM163 (0x15); // Bias (Spannung), {0 0 0 1 BS 1 0 Fx} = 00010101, es kann auch 1D gewählt werden, dann leuchtet der Hintergrund stark nach
	write_instruction_DOGM163 (0x7F); // Contrast (Bit C3-C0) {0 1 1 1 C3 C2 C1 C0} = 01111111
	write_instruction_DOGM163 (0x5F); // Power/ICON/Contrast (Bit C5-C4) {0 1 0 1 Ion Bon C5 C4} = 01011111
	write_instruction_DOGM163 (0x69); // Follower {0 1 1 0 Fon Rab2 Rab1 Rab0} = 01101001, Rab siehe Datenblatt Power supply
    _delay_ms(200);
	write_instruction_DOGM163 (0x0C); // Display Function, {0 0 0 0 1 D C B} = 00001100
	write_instruction_DOGM163 (0x06); // Entry Mode {0 0 0 0 0 1 I/D S} = 00000110


void init_doublehigh_lower_DOGM163(void) 
{
	init_PORTS_DOGM163();
	_delay_ms(200);
	write_instruction_DOGM163 (0x3E); // Instruction Set, {0 0 1 DL N DH IS2 IS1} = 00 11 11 10  
	write_instruction_DOGM163 (0x10); // Double hight position {0 0 0 1 DH x x x } 00 01 0x xx
 	write_instruction_DOGM163 (0x3D); // Instruction Set, {0 0 1 DL N DH IS2 IS1} = 00 11 11 10  
	write_instruction_DOGM163 (0x15); // Bias (Spannung), {0 0 0 1 BS 1 0 Fx} = 00010101, es kann auch 1D gewählt werden, dann leuchtet der Hintergrund stark nach
	write_instruction_DOGM163 (0x7F); // Contrast (Bit C3-C0) {0 1 1 1 C3 C2 C1 C0} = 01111111
	write_instruction_DOGM163 (0x5F); // Power/ICON/Contrast (Bit C5-C4) {0 1 0 1 Ion Bon C5 C4} = 01011111
	write_instruction_DOGM163 (0x69); // Follower {0 1 1 0 Fon Rab2 Rab1 Rab0} = 01101001, Rab siehe Datenblatt Power supply
    _delay_ms(200);
	write_instruction_DOGM163 (0x0C); // Display Function, {0 0 0 0 1 D C B} = 00001100
	write_instruction_DOGM163 (0x06); // Entry Mode {0 0 0 0 0 1 I/D S} = 00000110
}
*/

// setzt den Coursor an eine bestimmte Zeile und Spalte. Nullnotation beachten!
void set_cursor_DOGM163 (char row, char column)
{
	// Dislay Position ist Zeile Spalte der DDRAM Adresse. RS=0!
	// 00 01 02 03 .... 0F
	// 10 11 12 13 .... 1F
	// 20 21 22 23 .... 2F
	// DB7 = 1, DB6-DB0 = variable
	// Da, DB7 eine 1 ist wird immer 0x80 oder 128 dazu addiert (CHARACTER_BUFFER_BASE_ADDRESS)
	// Bsp. Zeile 2, Spalte 3 bedeutet 0x12, damit muss ans DDRAM 0x80 + 0x12 = 0x92 geschickt werden.
	// Eleganter geht es natürlich mit den beiden Konstanten, da eine neue Zeile genau nach der Zeilenlänge 
	// an der ersten Position beginnt. Aus 0x0F (Spalte 16) wird 0x10, Spalte und 0x80 dazu addiert und fertig.
	write_instruction_DOGM163 (CHARACTER_BUFFER_BASE_ADDRESS + row * CHARACTERS_PER_ROW + column);
}


void clear_DOGM163 (void) 
{
 	write_instruction_DOGM163 (0x01);
}


void cursor_home_DOGM163 (void) 
{
	write_instruction_DOGM163 (0x02);
}


//Display Function, {0 0 0 0 1 D C B} D=turned on/off, C=cursor on/off, B=blink on/off
void cursorON_blinkOFF_DOGM163 (void)
{
	write_instruction_DOGM163 (0x0E);
}



//Display Function, {0 0 0 0 1 D C B} D=turned on/off, C=cursor on/off, B=blink on/off
void cursorON_blinkON_DOGM163 (void)
{
	write_instruction_DOGM163 (0x0F);
}


//Display Function, {0 0 0 0 1 D C B} D=turned on/off, C=cursor on/off, B=blink on/off
void cursorOFF_blinkOFF_DOGM163 (void)
{
	write_instruction_DOGM163 (0x0C);
}



//Display Function, {0 0 0 0 1 D C B} D=turned on/off, C=cursor on/off, B=blink on/off
void cursorOFF_blinkON_DOGM163 (void)
{
	write_instruction_DOGM163 (0x0D);
}




//Display Function, {0 0 0 0 1 D C B} D=turned on/off, C=cursor on/off, B=blink on/off
void turnOFF_DOGM163 (void)
{
	write_instruction_DOGM163 (0x08);
}




void set_contrast_DOGM163 (uint8_t contrastvalue)
// es muss 2 mal der Kontrast gesetzt werden. Dies geht nur in der InstructionTabelle 1
// einmal über den Punkt "contrast set" die Bits C3 - C0 und dann im Power/ICON/Contrast die Bits C5-C4
{
	if (contrastvalue > 63) 
	{
		contrastvalue =63;
	}

	uint8_t lowerBits; //C3-C0
	uint8_t higherBits; //C5-C4

	lowerBits = contrastvalue & 0b00001111; // Maske nur für die 4 bits
	lowerBits = lowerBits | 0b01110000; // aufüllen von den restlichen Nullen
	
	write_instruction_DOGM163 (0x39); //
	write_instruction_DOGM163 (lowerBits); // Bits schreiben

	higherBits = contrastvalue >> 4; // C5-C4 4 mal nach rechts schieben damit sie auf Bit 0 und 1 kommen. 
	higherBits = higherBits | 0b01011100; //verODERn der restlichen Bits
	write_instruction_DOGM163 (higherBits); // Bits schreiben
	
}



void set_character_DOGM163 (uint8_t characterNumber, uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6, uint8_t data7)
{
	write_instruction_DOGM163 (0x38); // Instruction Set Tabelle 0, normal mode. Zeichen schreiben geht nur in Tabelle 0
	write_instruction_DOGM163 (0x40+(characterNumber<<3)); // set CGRAM Adress -> Character 0...7, <<3 da die Zeichenadresse bei b5-b3 steht und b2-b0 am Anfang auf null steht.
	write_char_DOGM163 (data0); //Pixel die gesetzt werden sollen
	write_char_DOGM163 (data1);
	write_char_DOGM163 (data2);
	write_char_DOGM163 (data3);
	write_char_DOGM163 (data4);
	write_char_DOGM163 (data5);
	write_char_DOGM163 (data6);
	write_char_DOGM163 (data7);
	write_instruction_DOGM163 (0x39); // Instruction Set Tabelle 1

	// jetzt muss nur die richtige DDRAM Adresse gesetzt werden, damit das neue Zeichen auch auf dem Display erscheint. 
	// Wird gemacht im Hauptprogramm mit Zeile/Spalte setzen oder cursor home

}




