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


/**********************************************************************/
/* Externally defined functions */
extern void gate_array_call(const uint8_t x);
extern void ppi_write_port_a(const uint8_t v);
extern void ppi_write_port_b(const uint8_t v);
extern void ppi_write_port_c(const uint8_t v);
extern void ppi_control(const uint8_t v);
extern void crtc_select_register(const uint8_t r);
extern void crtc_write_register(const uint8_t v);
extern uint8_t crtc_read_register();
extern uint8_t ppi_read_port_a();
extern uint8_t ppi_read_port_b();
extern uint8_t ppi_read_port_c();

/* Internally defined functions */
void io_write(const uint16_t addr);
void io_read(const uint16_t addr, uint8_t * v);

/**********************************************************************/


/**********************************************************************/
/*   IO                                                               */
/**********************************************************************/

void io_write( const uint16_t addr) {

	uint8_t v = addr & 0x00FF;
	switch ( addr >> 8 ) {
		
		case 0x7F:	/* gate array */
					gate_array_call(v); 
					break;
		case 0xF4: /* ppi port A */
					ppi_write_port_a(v);
					break;
		case 0xF5: /* ppi port B */
					ppi_write_port_b(v);
					break;
		case 0xF6: /* ppi port C */
					ppi_write_port_c(v);			
					break;
		case 0xF7: /* ppi control */
					ppi_control( v );
					break;		
		case 0xEF: 	/* printer port */
					/* not implemented */
					break;
		case 0xBC:	/* crtc register select */
					crtc_select_register(v);
					break;
		case 0xBD:	/* crtc */
					crtc_write_register(v);
					break;
		case 0xDF:	/* expansion rom */
					/* we are not implementing expansion roms */
					break;
		case 0xF8:	/* expansion peripherals */
		case 0xF9:
		case 0xFA:
		case 0xFB:  /* not implemented */
					break;
		default:	printf("!!!!MISSING OUTPUT PORT %04x\n",addr);
				
					break;
			
			
	}
	
}

void io_read( const uint16_t addr, uint8_t * v) {
	
	/* printf("IO READ: %04X, %02X\n", addr, *v); */
	
	switch ( (addr >> 8) ) {
		
		case 0xF4: /* ppi port A */
					*v = ppi_read_port_a();
					break;
		case 0xF5: /* ppi port B */
					*v = ppi_read_port_b();
					break;
		case 0xF6: /* ppi port C */
					*v = ppi_read_port_c();
					break;
		
		/* case 0xbc and 0xbd: crtc */
		/* If you perform an IN instruction to the select or write functions 
		 * it will write data to the CRTC from the current data on the bus */
		/* I am not implementing this because I do not know which bus refers to */
		case 0xBE: 	/* crtc do not use for output */
					/* reserved for crtc status, it does not work on all the CRTC */
					*v = 0x00;
					break;
		case 0xBF:	/* crtc do not use for output */
					/* hd6845 crtc data , only works on type 0 crtc*/
					*v = crtc_read_register();
					break;
		case 0xF8:	/* expansion peripherals */
		case 0xF9:
		case 0xFA:
		case 0xFB:  /* not implemented */
					*v = 0x00;
					break;
		default:	printf("!!!!MISSING INPUT PORT %04x\n",addr);
					break;
		
		
	}
	
}


