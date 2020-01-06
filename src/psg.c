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

#define NUM_PSG_REGISTERS 16
#define PSG_PORT_A 14
#define PSG_PORT_B 15 /* I think this is not used on CPC6128 */


static	uint8_t psgregs[NUM_PSG_REGISTERS];
static  uint8_t psg_register_selected = 0;
	
/**********************************************************************/
/* externally defined functions */
extern uint8_t ppi_read_port_a();
extern void ppi_write_port_a(uint8_t v);
	
/**********************************************************************/
void psg_read_keyboard(uint8_t l) {
	psgregs [ PSG_PORT_A ] = l;
}

void psg_select_register() {
	psg_register_selected = ppi_read_port_a();
}

void psg_write_register() {
	psgregs [ psg_register_selected ] = ppi_read_port_a();
}

void psg_read_register() {
	
	switch ( psg_register_selected ) {
		case 1:
		case 3:
		case 5:
		case 13:
			ppi_write_port_a ( psgregs [ psg_register_selected ] & 0x0F );
			break;
		case 6:
		case 8:
		case 9:
		case 10:
			ppi_write_port_a ( psgregs [ psg_register_selected ] & 0x1F );
			break;
		default: 
			ppi_write_port_a ( psgregs [ psg_register_selected ]  );
			break;
	}
}

	
/**********************************************************************/
void psg_command(uint8_t c) {

	switch (c) {
		case 0:	return; break;
		case 1: psg_read_register(); break; 
		case 2: psg_write_register(); break;
		case 3: psg_select_register(); break;
	}
}






