/* Copyright (c) 2017 Cesar Lombao
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

#ifndef _AMSTRAD_H
#define _AMSTRAD_H

#include <lx80.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#define MAX_PENS 16

#define LOWER_ROM 0
#define UPPER_ROM 1

#define OUTPUT 0
#define INPUT 1

#define ENABLED 	1
#define DISABLED	0

typedef struct {
	int screen_mode;
	int pen_selected;			/* -1 means Border */
	int pens_colour[MAX_PENS];
	int border_colour;
	
	int lower_rom_area;
	int upper_rom_area;
	
	int expansion_rom_selected;
	
	int ram_page_mode;
	
	uint8_t ram[64*1024];
	uint8_t lower_rom[16*1024];
	uint8_t upper_rom[16*1024];

	#define NUM_PSG_REGISTERS 16
	#define PSG_PORT_A 14
	struct {
		uint8_t regs[NUM_PSG_REGISTERS];
		uint8_t reg_selected;
	
	} psg;
	
	struct {
		
		struct {
			uint8_t data;
			uint8_t direction;
			uint8_t mode;
		} porta; /* psg data bus */
		
		
		struct {
			uint8_t mode;
			uint8_t direction; /* 0 output 1 input */
			
			union {
				uint8_t data;
				struct  {
					uint8_t CRTC: 1;
					uint8_t LK1: 1;
					uint8_t LK2: 1;
					uint8_t LK3: 1;
					uint8_t LK4: 1;
					uint8_t EXP: 1;
					uint8_t PRNBUSY1;
					uint8_t CASIN: 1; 
				};
			};
		} portb;
		
		
		struct {
			uint8_t mode;
			uint8_t direction; /* 1 input 0 output */
			
			union {
				uint8_t data;
				struct  {
					uint8_t PSGBDIR:1;
					uint8_t PSGBC1:1;
					uint8_t CasWriteData:1;
					uint8_t CasMotorControl:1;
					uint8_t KLine:4;
				};
			};
		} portc;
		
		
	} ppi;
	
	#define CRTC_NUM_REGS 16
	struct {
		uint8_t reg[CRTC_NUM_REGS];
	} crtc;
    uint8_t crtc_register_selected;
	
	#define KEYBOARD_MATRIX_LINES 16
	struct {
		uint8_t line[KEYBOARD_MATRIX_LINES];
	} keyboard;

	uint8_t printerport;
	
	struct {
		SDL_Window  	* window;
		SDL_Renderer 	* renderer;	
		SDL_Point pixels[640*200];
	} screen;
	
	
	t_z80 * z80;
	
} t_amstrad;


#endif
