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
#include <stdlib.h>
#include <stdio.h>

#include "amstrad.h"


extern t_amstrad amstrad;


/**********************************************************************/
/* INTERNALLY DEFINED FUNCTIONS										  */
void video_ram(uint16_t addr);	
void write_ram ( const uint16_t addr, const uint8_t * v );
void read_ram ( const uint16_t addr, uint8_t * v );
void upload_rom(char * filepath, int rom);
/**********************************************************************/


void read_ram ( const uint16_t addr, uint8_t * v ) {
	
	switch ( addr >> 14 ) {
		case 0:	if ( amstrad.lower_rom_area == DISABLED ) { *v = amstrad.ram[addr]; }
				else {  *v = amstrad.lower_rom[addr]; } 
				break;
		case 1: *v = amstrad.ram[addr];
				break;
		case 2: *v = amstrad.ram[addr];
				break;
		case 3:	if ( amstrad.upper_rom_area == DISABLED ) { *v = amstrad.ram[addr]; } 
				else {  *v = amstrad.upper_rom[addr & 0x3FFF ]; }
				break;	
	}	
}

void write_ram ( const uint16_t addr, const uint8_t * v ) {
	amstrad.ram[addr] = *v;
}



void upload_rom(char * filepath, int rom) {
	
	FILE * fd=fopen(filepath, "rb"); 
	if ( fd == NULL) { 
		perror("Could not upload ROM file\n");
		exit(EXIT_FAILURE);
	}

	uint8_t * ptr = NULL;
	switch (rom) {
		case LOWER_ROM: ptr = amstrad.lower_rom; break;
		case UPPER_ROM: ptr = amstrad.upper_rom; break;
		default: perror("Undefined ROM Value"); exit(EXIT_FAILURE); break;
	}
	
	int 	n=fread(ptr,1,16*1024,fd);
	
	if ( n != 16*1024) {
		perror("Wrong number of bytes in lower ROM\n");
		exit(EXIT_FAILURE);
	}
	fclose(fd);
	
}


/**************************************************************/

void video_ram(uint16_t addr) {
	
	
	uint16_t k = addr & 0x3FFF;
	uint8_t v = amstrad.ram[addr];
	
	int n = k / 0x800; 
	int l = (k - n*0x800) / 0x50;
	int y = l*8 + n;
	int x;
	
	int p = (k - n*0x800) - (0x50*l);
	
	uint8_t n1; uint8_t n2;
	switch(amstrad.screen_mode) {
		case 1:	x = p * 4; 
		
				printf("SCR MODE: %d  .Para el addr: %04X nos da la linea(x,y): %d,%d, valor:%d\n",amstrad.screen_mode,addr,x,y,v);
	
				n1 = v & 0x0F; n2 = ((v >> 4) & 0x0F );
				if ( ((n2 & 0x01) | ((n1 & 0x01) << 1)) == 0 ) { 	SDL_SetRenderDrawColor(amstrad.screen.renderer,0,0,255,SDL_ALPHA_OPAQUE);	}
				else          {     SDL_SetRenderDrawColor(amstrad.screen.renderer,255,255,0,SDL_ALPHA_OPAQUE);	}
				SDL_RenderDrawPoint(amstrad.screen.renderer,x+3,y); 
			/*	 
				n1 = n1 >> 1; n2 = n2 >> 1;
				if ( ((n2 & 0x01) | ((n1 & 0x01) << 1)) == 0 ) { 	SDL_SetRenderDrawColor(amstrad.screen.renderer,0,0,255,SDL_ALPHA_OPAQUE);	}
				else          {     SDL_SetRenderDrawColor(amstrad.screen.renderer,255,255,0,SDL_ALPHA_OPAQUE);	}
				SDL_RenderDrawPoint(amstrad.screen.renderer,x+2,y);
				
				n1 = n1 >> 1; n2 = n2 >> 1;
				if ( ((n2 & 0x01) | ((n1 & 0x01) << 1)) == 0 ) { 	SDL_SetRenderDrawColor(amstrad.screen.renderer,0,0,255,SDL_ALPHA_OPAQUE);	}
				else          {     SDL_SetRenderDrawColor(amstrad.screen.renderer,255,255,0,SDL_ALPHA_OPAQUE);	}
				SDL_RenderDrawPoint(amstrad.screen.renderer,x+1,y);
				
				n1 = n1 >> 1; n2 = n2 >> 1;
				if ( ((n2 & 0x01) | ((n1 & 0x01) << 1)) == 0 ) { 	SDL_SetRenderDrawColor(amstrad.screen.renderer,0,0,255,SDL_ALPHA_OPAQUE);	}
				else          {     SDL_SetRenderDrawColor(amstrad.screen.renderer,255,255,0,SDL_ALPHA_OPAQUE);	}
				SDL_RenderDrawPoint(amstrad.screen.renderer,x,y);
			*/
				SDL_RenderPresent(amstrad.screen.renderer);	
				break;
								
		case 2:	x = p * 8; break;
		case 0:	x = p * 2; break;
	}
	
	
}
