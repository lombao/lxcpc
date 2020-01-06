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

static struct {
		
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



/**********************************************************************/
/* Externally defined functions */
void psg_read_register();
void psg_write_register();
void psg_select_register();

/* Internally defined functions */
void ppi_write_port_a(const uint8_t v);
void ppi_write_port_b(const uint8_t v);
void ppi_write_port_c(const uint8_t v);
void ppi_control(const uint8_t v);
uint8_t ppi_read_port_a();
uint8_t ppi_read_port_b();
uint8_t ppi_read_port_c();
void ppi_init();

/**********************************************************************/


void ppi_init() {
	
	ppi.portb.data = 0xFE; /* set vsync 0 */
	ppi.porta.data = 0x00;	
	ppi.porta.mode = 0x00;
	ppi.portb.mode = 0x00;
	ppi.portc.mode = 0x00;
	ppi.porta.direction = 1;
	ppi.portb.direction = 1;
	ppi.portc.direction = 0;
	
}

/**********************************************************************/
void ppi_write_port_a(const uint8_t v) { ppi.porta.data = v; }
void ppi_write_port_b(const uint8_t v) { ppi.portb.data = v; }

void ppi_write_port_c(const uint8_t v) { 
	ppi.portc.data = v; 
	switch ( v >> 6 ) {
		case 0: /*inactive*/
				break;
		case 1: /* read from selected PSG reg */
				psg_read_register();
				break;
		case 2: /* write to selected psg reg */
				psg_write_register();
				break;
		case 3: /* select psg register */
				psg_select_register();
				break;
	}
			
}

uint8_t ppi_read_port_a() { return ppi.porta.data; }
uint8_t ppi_read_port_b() { return (ppi.portb.data | 0x1E); } /* this is to return always Lk1, lk2, and lk3 set 1 and 50 Hz */
uint8_t ppi_read_port_c() { return ppi.portc.data; }

 
/*************************************/
void ppi_control(const uint8_t v) {

uint8_t nbit;
uint8_t t;
	
	switch ( v >> 7 ) {
		case 0: /* bit 7 0ff */
				nbit = (v >> 1) & 0x07;
				t = (1 << nbit);
				if ( v & 0x01 ) { ppi.portc.data |= t; }
				else         	{ ppi.portc.data &= ~t; }
							
				/* MISSING , Something must happen 
				 * if a bit is changed in the register 
				 */				
				break;
		case 1: /* bit 7 on */
				/* I've read that in the CPC's only bit 4 is relevant */
				ppi.porta.direction = ( v >> 4 ) & 0x01;
				ppi.portc.direction = ( v >> 3 ) & 0x01;
				ppi.porta.mode 		= ( v >> 5 ) & 0x01;
				ppi.portc.mode 		= ( v >> 6 ) & 0x01;
				ppi.portb.direction = ( v >> 1 ) & 0x01;
				ppi.portb.mode 		= ( v >> 2 ) & 0x01;
				
				break;
		default: /* cannot be */
				perror("Imposible in PPI\n");
				exit(EXIT_FAILURE);
				break;
	}
}

