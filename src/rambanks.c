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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ENABLED  1
#define DISABLED 0

/* DOCUMENTATION RAM Page modes */
/*
Configuration "0":

Z80 Memory Range 	Sub-block
&0000-&3fff 	0
&4000-&7fff 	1
&8000-&bfff 	2
&c000-&ffff 	3

Configuration "1":

Z80 Memory Range 	Sub-block
&0000-&3fff 	0
&4000-&7fff 	1
&8000-&bfff 	2
&c000-&ffff 	3*

Configuration "2":

Z80 Memory Range 	Sub-block
&0000-&3fff 	0*
&4000-&7fff 	1*
&8000-&bfff 	2*
&c000-&ffff 	3*

Configuration "3":

Z80 Memory Range 	Sub-block
&0000-&3fff 	0
&4000-&7fff 	3
&8000-&bfff 	2
&c000-&ffff 	3*

Configuration "4":

Z80 Memory Range 	Sub-block
&0000-&3fff 	0
&4000-&7fff 	0*
&8000-&bfff 	2
&c000-&ffff 	3

Configuration "5":

Z80 Memory Range 	Sub-block
&0000-&3fff 	0
&4000-&7fff 	1*
&8000-&bfff 	2
&c000-&ffff 	3

Configuration "6"

Z80 Memory Range 	Sub-block
&0000-&3fff 	0
&4000-&7fff 	2*
&8000-&bfff 	2
&c000-&ffff 	3

Configuration "7":

Z80 Memory Range 	Sub-block
&0000-&3fff 	0
&4000-&7fff 	3*
&8000-&bfff 	2
&c000-&ffff 	3
*/





/* In CPC6128 we have 2 RAM banks of 64K */
/* Each bank consist in 4 blocks of 16K each */
#define NUM_RAM_BANKS 2 
#define NUM_BLOCKS 4
struct {
	struct {
		uint8_t mem[ 16 * 1024];
	} blocks[ NUM_BLOCKS ];
} ram[ NUM_RAM_BANKS ];

struct {
	uint8_t lower_rom[16 * 1024];
	uint8_t upper_rom[16 * 1024];
} rom;


/**********************************************************/
/* Externally defined functions */
extern int gate_array_lower_rom_status();
extern int gate_array_upper_rom_status();
extern int gate_array_ram_page_mode();

/* internally defined functions */
void read_ram ( const uint16_t addr, uint8_t * v );
uint8_t access_ram_video(uint16_t addr);
void write_ram ( const uint16_t addr, const uint8_t * v );
void upload_roms();

/**********************************************************/

void read_ram ( const uint16_t addr, uint8_t * v ) {
	
	int block = addr >> 14;
	
	if ( block == 0 && gate_array_lower_rom_status() == ENABLED ) {
		*v = rom.lower_rom[addr];
		return;
	}
	if ( block == 3 && gate_array_upper_rom_status() == ENABLED ) {
		*v = rom.upper_rom[addr & 0x3FFF];
		return;
	}
	
	switch ( gate_array_ram_page_mode() ) {
		case 0: *v = ram[0].blocks[ block ].mem[ addr & 0x3FFF ]; break;
		
		case 1: switch ( block ) {
					case 0: *v = ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ]; break;
					case 1: *v = ram[0].blocks[ 1 ].mem[ addr & 0x3FFF ]; break;
					case 2: *v = ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ]; break;
					case 3: *v = ram[1].blocks[ 3 ].mem[ addr & 0x3FFF ]; break;
				}
				break;
		case 2: *v = ram[1].blocks[ block ].mem[ addr & 0x3FFF ]; break;
		
		case 3: switch ( block ) {
					case 0: *v = ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ]; break;
					case 1: *v = ram[0].blocks[ 3 ].mem[ addr & 0x3FFF ]; break;
					case 2: *v = ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ]; break;
					case 3: *v = ram[1].blocks[ 3 ].mem[ addr & 0x3FFF ]; break;
				}
				break;
		case 4: switch ( block ) {
					case 0: *v = ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ]; break;
					case 1: *v = ram[1].blocks[ 0 ].mem[ addr & 0x3FFF ]; break;
					case 2: *v = ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ]; break;
					case 3: *v = ram[0].blocks[ 3 ].mem[ addr & 0x3FFF ]; break;
				}
				break;
		case 5: switch ( block ) {
					case 0: *v = ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ]; break;
					case 1: *v = ram[1].blocks[ 1 ].mem[ addr & 0x3FFF ]; break;
					case 2: *v = ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ]; break;
					case 3: *v = ram[0].blocks[ 3 ].mem[ addr & 0x3FFF ]; break;
				}
				break;
		case 6: switch ( block ) {
					case 0: *v = ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ]; break;
					case 1: *v = ram[1].blocks[ 2 ].mem[ addr & 0x3FFF ]; break;
					case 2: *v = ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ]; break;
					case 3: *v = ram[0].blocks[ 3 ].mem[ addr & 0x3FFF ]; break;
				}
				break;
		case 7: switch ( block ) {
					case 0: *v = ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ]; break;
					case 1: *v = ram[1].blocks[ 3 ].mem[ addr & 0x3FFF ]; break;
					case 2: *v = ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ]; break;
					case 3: *v = ram[0].blocks[ 3 ].mem[ addr & 0x3FFF ]; break;
				}
				break;
				
	}
	
}


/**********************************************************/

void write_ram ( const uint16_t addr, const uint8_t * v ) {

	int block = addr >> 14;
	
	switch ( gate_array_ram_page_mode() ) {
		case 0: ram[0].blocks[ block ].mem[ addr & 0x3FFF ] = *v; break;
		
		case 1: switch ( block ) {
					case 0: ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ] = *v; break;
					case 1: ram[0].blocks[ 1 ].mem[ addr & 0x3FFF ] = *v; break;
					case 2: ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ] = *v; break;
					case 3: ram[1].blocks[ 3 ].mem[ addr & 0x3FFF ] = *v; break;
				}
				break;
		case 2: ram[1].blocks[ block ].mem[ addr & 0x3FFF ] = *v; break;
		
		case 3: switch ( block ) {
					case 0: ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ] = *v; break;
					case 1: ram[0].blocks[ 3 ].mem[ addr & 0x3FFF ] = *v; break;
					case 2: ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ] = *v; break;
					case 3: ram[1].blocks[ 3 ].mem[ addr & 0x3FFF ] = *v; break;
				}
				break;
		case 4: switch ( block ) {
					case 0: ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ] = *v; break;
					case 1: ram[1].blocks[ 0 ].mem[ addr & 0x3FFF ] = *v; break;
					case 2: ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ] = *v; break;
					case 3: ram[0].blocks[ 3 ].mem[ addr & 0x3FFF ] = *v; break;
				}
				break;
		case 5: switch ( block ) {
					case 0: ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ] = *v; break;
					case 1: ram[1].blocks[ 1 ].mem[ addr & 0x3FFF ] = *v; break;
					case 2: ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ] = *v; break;
					case 3: ram[0].blocks[ 3 ].mem[ addr & 0x3FFF ] = *v; break;
				}
				break;
		case 6: switch ( block ) {
					case 0: ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ] = *v; break;
					case 1: ram[1].blocks[ 2 ].mem[ addr & 0x3FFF ] = *v; break;
					case 2: ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ] = *v; break;
					case 3: ram[0].blocks[ 3 ].mem[ addr & 0x3FFF ] = *v; break;
				}
				break;
		case 7: switch ( block ) {
					case 0: ram[0].blocks[ 0 ].mem[ addr & 0x3FFF ] = *v; break;
					case 1: ram[1].blocks[ 3 ].mem[ addr & 0x3FFF ] = *v; break;
					case 2: ram[0].blocks[ 2 ].mem[ addr & 0x3FFF ] = *v; break;
					case 3: ram[0].blocks[ 3 ].mem[ addr & 0x3FFF ] = *v; break;
				}
				break;
				
	}
}

/**********************************************************************/
/* video ram will be only in the bank 0    							  */
uint8_t access_ram_video(uint16_t addr) {
	int block = addr >> 14;
	return ram[0].blocks[ block ].mem[ addr & 0x3FFF ];
}

/**********************************************************************/
/* upload roms from disk */
void upload_roms() {
	
char filepath[300];
char osrom[300];
char basicrom[300];

#ifndef DATASOURCEDIR
#define DATASOURCEDIR "/usr/local/share/lx80"
#endif

	strcpy(filepath,DATASOURCEDIR);
	strcat(filepath,"/rom");
	
	strcpy(osrom,filepath);
	strcat(osrom,"/os6128.rom");
	
	strcpy(basicrom,filepath);
	strcat(basicrom,"/basic6128.rom");
	
	
	FILE * fd=fopen(osrom, "rb"); 
	if ( fd == NULL) { 
		fprintf(stderr,"Could not upload OS ROM file: %s\n",osrom);
		exit(EXIT_FAILURE);
	}

	int	n=fread(rom.lower_rom,1,16*1024,fd);
	if ( n != 16*1024) {
		fprintf(stderr,"Wrong number of bytes in lower ROM\n");
		exit(EXIT_FAILURE);
	}
	fclose(fd);

	fd=fopen(basicrom, "rb"); 
	if ( fd == NULL) { 
		fprintf(stderr,"Could not upload BASIC ROM file: %s\n",basicrom);
		exit(EXIT_FAILURE);
	}

	n=fread(rom.upper_rom,1,16*1024,fd);
	if ( n != 16*1024) {
		perror("Wrong number of bytes in upper ROM\n");
		exit(EXIT_FAILURE);
	}
	fclose(fd);

}
