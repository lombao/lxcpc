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
#include <signal.h>
#include <lx80.h>
#include <SDL2/SDL.h>
#include <stdio.h>

/**********************************************************************/
/* EXTERNALLY DEFINED FUNCTIONS										  */
extern void upload_roms();
extern void ppi_init();
extern void io_write(const uint16_t addr);
extern void io_read(const uint16_t addr, uint8_t * v);
extern void write_ram ( const uint16_t addr, const uint8_t * v );
extern void read_ram ( const uint16_t addr, uint8_t * v );
extern void window_init();
extern void window_destroy();
extern void crtc_set_hsync_timer();
extern void keyboard_timer_scan();
extern void video_createframe();
/**********************************************************************/

/**********************************************************************/
/* INTERNALLY DEFINED FUNCTIONS										  */
void program_quit();

/**********************************************************************/



/***********************************************/
/* Quit in an orderly fashion                  */
void program_quit() {
	window_destroy();
	exit(EXIT_SUCCESS);
}


int main() {

	upload_roms();
	ppi_init();	
		
	/* initialize the z80 */		
	z80_init(4, io_read, io_write, read_ram, write_ram );
	
	/* initialize SDL */
	window_init();
	
	/* capture signals */ 
	signal(SIGTERM, program_quit);
	signal(SIGINT, program_quit);
	
	/* set the timer, interruption */
	crtc_set_hsync_timer();

	/* set the timer for keyboard scan */
	keyboard_timer_scan();

    /* Simply create a thread */
    SDL_Thread * thread = SDL_CreateThread(z80_run, "z80", (void *)NULL); 

    if (NULL == thread) {
        fprintf(stderr,"SDL_CreateThread failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    } else {
		while(1) {
			video_createframe();
			SDL_Delay(40);
		}
    }

	program_quit();

}








