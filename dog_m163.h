#ifndef _dog_m163_h_
#define _dog_m163_h_



void init_PORTS_DOGM163 (void);

void reset_CSB_DOGM163 (void);

void write_instruction_DOGM163 (char instruction);

void write_char_DOGM163 (char instruction);

void write_string_DOGM163 (char *buffer);

void init_3lines_DOGM163(void);

void init_3lines_DOGM163_v2(void);

void init_doublehigh_lower_DOGM163(void);

void set_cursor_DOGM163 (char row, char column);

void clear_DOGM163 (void);

void cursor_home_DOGM163 (void);

void cursorON_blinkOFF_DOGM163 (void);

void cursorON_blinkON_DOGM163 (void);

void cursorOFF_blinkOFF_DOGM163 (void);

void cursorOFF_blinkON_DOGM163 (void);

void turnOFF_DOGM163 (void);

void set_contrast_DOGM163 (uint8_t contrastvalue);

void set_character_DOGM163 (uint8_t characterNumber, uint8_t data0, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6, uint8_t data7);

#endif
