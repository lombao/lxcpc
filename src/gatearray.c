/* Copyright (c) 2019 Cesar Lombao
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
 
#include <stdint.h>
#include <lxz80.h>

#define ENABLED 	1
#define DISABLED	0

/* The function to be performed is selected by writing data to the Gate-Array,
 *  bit 7 and 6 of the data define the function selected (see table below).
 *  It is not possible to read from the Gate-Array.

Bit 7 	Bit 6 	Function
0 	0 	Select pen
0 	1 	Select colour for selected pen
1 	1 	Select screen mode, rom configuration and interrupt control
1 	1 	Ram Memory Management (note 1)
*/


/* Note, the PEN 16 is actually the border */
#define MAX_PENS 16
#define BORDER MAX_PENS
static uint8_t pen_selected 	= 0;
static uint8_t pens[MAX_PENS + 1];
static uint8_t screen_mode = 1;
static uint8_t upper_rom_area = ENABLED;
static uint8_t lower_rom_area = ENABLED;  
static uint8_t ram_page_mode = 0;
static uint8_t hsync_counter = 0;


/**********************************************************************/
/* Externall defined functions */
extern void z80_signal_int();
		
/* Internally defined functions */
void gate_array_call(const uint8_t x);
void gate_array_hsync();
int gate_array_lower_rom_status();
int gate_array_upper_rom_status();
int gate_array_ram_page_mode();
int gate_array_screen_mode();

/**********************************************************************/



/* Interface to the Gate Array */
void gate_array_call(const uint8_t x) {
	
	switch(  x >> 6 ) {
		case 0: if ( x & 0x10 ) { pen_selected 		= BORDER; }
				else 			{ pen_selected 		= x & 0x0F;	}
				break;
		case 1: pens[ pen_selected ]  = x & 0x1F; 
				break;
		case 2: screen_mode =  x & 0x03 ; 
				if ( x & 0x08 ) { upper_rom_area = DISABLED; } else { upper_rom_area = ENABLED; }
				if ( x & 0x04 ) { lower_rom_area = DISABLED; } else { lower_rom_area = ENABLED; }
				break;
		case 3: 
				ram_page_mode = (x & 0x07);
				break;
	}
}



/****************************************/
void gate_array_hsync() {
	/* THIS IS NOT COMPLETE !!!! */
	if ( ++hsync_counter == 52 ) {
		z80_signal_int();
		hsync_counter = 0;
	}
	
}

/*****************************************/
int gate_array_lower_rom_status() {
	return lower_rom_area;
}
int gate_array_upper_rom_status() {
	return upper_rom_area;
}
int gate_array_ram_page_mode() {
	return ram_page_mode;
}
int gate_array_screen_mode() {
	return screen_mode;
}
int gate_array_pen_color(uint8_t pen) {
	return pens[pen];
}
