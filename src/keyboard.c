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
#include <SDL2/SDL.h>
 
 
 
#define KEYBOARD_MATRIX_LINES 16
static uint8_t keylines[KEYBOARD_MATRIX_LINES] = {
	0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF,
	0xFF,0xFF,0xFF,0xFF
};
static SDL_TimerID keyboard_timer_id;

/**********************************************************************/
/* externally defined function                                        */
extern void psg_read_keyboard(uint8_t l);

/* internally defined function 										  */
void keyboard_timer_scan();
uint32_t keyboard_handler(uint32_t interval,void * d);
void keyboard_press(SDL_Keysym * k);
void keyboard_release(SDL_Keysym * k);


/**********************************************************************/
/* this is called trouugh ppi c by the defined scanlines              */
void keyboard_set_scanline(uint8_t l) {
	psg_read_keyboard (keylines[l]);
}


/**********************************************************************/
/* sets a timer that exeutes the handler in order to scan the keyboard*/
void keyboard_timer_scan() {
	
	/* we can every 1Mhz = 1 milisecond */
	keyboard_timer_id = SDL_AddTimer(1, keyboard_handler, NULL);

}


/**********************************************************************/
uint32_t keyboard_handler(uint32_t interval,void * d) {
	
	SDL_Event event;
	if (SDL_PollEvent(&event)  ) {
		switch(event.type) {
			case SDL_KEYDOWN: 	keyboard_press(&event.key.keysym); break;
			case SDL_KEYUP:		keyboard_release(&event.key.keysym); break;	
		}
	}
	
	return interval;
}

/**********************************************************************/
void keyboard_press(SDL_Keysym * k) {
		
	switch(k->sym) {

		case SDLK_EQUALS: keylines[3] &= 0xFD;	keylines[2] &= 0xDF;	break;		
		case SDLK_PLUS: keylines[3] &= 0xEF; keylines[2] &= 0xDF; break;
								
		case SDLK_LSHIFT: 
		case SDLK_RSHIFT: keylines[2] &= 0xDF;	;break;
		case SDLK_COLON: keylines[3] &= 0xDF;	;break;
		case SDLK_k: keylines[4] &= 0xDF;	;break;
		case SDLK_j: keylines[5] &= 0xDF;	;break;
		case SDLK_f: keylines[6] &= 0xDF;	;break;
		case SDLK_d: keylines[7] &= 0xDF;	;break;
		case SDLK_a: keylines[8] &= 0xDF;	;break;
							
		case SDLK_SEMICOLON: keylines[3] &= 0xEF;	;break;
		case SDLK_l: keylines[4] &= 0xEF;	;break;
		case SDLK_h: keylines[5] &= 0xEF;	;break;
		case SDLK_g: keylines[6] &= 0xEF;	;break;
		case SDLK_s: keylines[7] &= 0xEF;	;break;
		case SDLK_TAB: keylines[8] &= 0xEF;	;break;
		
		case SDLK_SPACE: keylines[5] &= 0x7F;	;break;
		case SDLK_v: keylines[6] &= 0x7F;	;break;
		case SDLK_x: keylines[7] &= 0x7F;	;break;
		case SDLK_z: keylines[8] &= 0x7F;	;break;
		case SDLK_BACKSPACE: keylines[9] &= 0x7F;	;break;
				
		case SDLK_RETURN: keylines[0] &= 0xBF;	;break;
		case SDLK_m: keylines[4] &= 0xBF;	;break;
		case SDLK_n: keylines[5] &= 0xBF;	;break;
		case SDLK_b: keylines[6] &= 0xBF;	;break;
		case SDLK_c: keylines[7] &= 0xBF;	;break;
		case SDLK_CAPSLOCK: keylines[8] &= 0xBF;	;break;
													
		case SDLK_p: keylines[3] &= 0xF7;	;break;
		case SDLK_i: keylines[4] &= 0xF7;	;break;
		case SDLK_y: keylines[5] &= 0xF7;	;break;
		case SDLK_t: keylines[6] &= 0xF7;	;break;
		case SDLK_w: keylines[7] &= 0xF7;	;break;
		case SDLK_q: keylines[8] &= 0xF7;	;break;
		
		case SDLK_DOWN: keylines[0] &= 0xFB;	;break;
		case SDLK_o: keylines[4] &= 0xFB;	;break;
		case SDLK_u: keylines[5] &= 0xFB;	;break;
		case SDLK_r: keylines[6] &= 0xFB;	;break;
		case SDLK_e: keylines[7] &= 0xFB;	;break;
		
		case SDLK_RIGHT: keylines[0] &= 0xFD;	;break;	
		case SDLK_MINUS: keylines[3] &= 0xFD;	;break;		
		case SDLK_9: keylines[4] &= 0xFD;	;break;	
		case SDLK_7: keylines[5] &= 0xFD;	;break;	
		case SDLK_5: keylines[6] &= 0xFD;	;break;	
		case SDLK_3: keylines[7] &= 0xFD;	;break;	
		case SDLK_2: keylines[8] &= 0xFD;	;break;
								
		case SDLK_UP: keylines[0] &= 0xFE;	;break;
		case SDLK_LEFT: keylines[1] &= 0xFE;	;break;
		case SDLK_0: keylines[4] &= 0xFE;	;break;	
		case SDLK_8: keylines[5] &= 0xFE;	;break;	
		case SDLK_6: keylines[6] &= 0xFE;	;break;	
		case SDLK_4: keylines[7] &= 0xFE;	;break;	
		case SDLK_1: keylines[8] &= 0xFE;	;break;

	}
}


/**********************************************************************/
void keyboard_release(SDL_Keysym * k) {
		
	switch(k->sym) {
		
		case SDLK_EQUALS: keylines[3] |= 0x02;	keylines[2] |= 0x20; break;		
		case SDLK_PLUS: keylines[3] |= 0x10;	keylines[2] |= 0x20; break;		
						
		case SDLK_LSHIFT: 
		case SDLK_RSHIFT: keylines[2] |= 0x20;	;break;
		case SDLK_COLON: keylines[4] |= 0x20;	break;
		case SDLK_k: keylines[4] |= 0x20;	break;
		case SDLK_j: keylines[5] |= 0x20;	break;
		case SDLK_f: keylines[6] |= 0x20;	break;
		case SDLK_d: keylines[7] |= 0x20;	break;
		case SDLK_a: keylines[8] |= 0x20; 	break;
					
		case SDLK_SEMICOLON: keylines[3] |= 0x10;	;break;							
		case SDLK_l: keylines[4] |= 0x10;	;break;
		case SDLK_h: keylines[5] |= 0x10;	;break;
		case SDLK_g: keylines[6] |= 0x10;	;break;
		case SDLK_s: keylines[7] |= 0x10;	;break;
		case SDLK_TAB: keylines[8] |= 0x10;	;break;
							
		case SDLK_SPACE: keylines[5] |= 0x80;	;break;
		case SDLK_v: keylines[6] |= 0x80;	;break;
		case SDLK_x: keylines[7] |= 0x80;	;break;
		case SDLK_z: keylines[8] |= 0x80;	;break;
		case SDLK_BACKSPACE: keylines[9] |= 0x80;	;break;
						
		case SDLK_RETURN: keylines[0] |= 0x40;	;break;
		case SDLK_m: keylines[4] |= 0x40;	break;
		case SDLK_n: keylines[5] |= 0x40;	break;
		case SDLK_b: keylines[6] |= 0x40;	break;
		case SDLK_c: keylines[7] |= 0x40;	break;
		case SDLK_CAPSLOCK: keylines[8] |= 0x40;	break;
							
		case SDLK_p: keylines[3] |= 0x08;	;break;
		case SDLK_i: keylines[4] |= 0x08;	;break;
		case SDLK_y: keylines[5] |= 0x08;	;break;
		case SDLK_t: keylines[6] |= 0x08;	;break;
		case SDLK_w: keylines[7] |= 0x08;	;break;
		case SDLK_q: keylines[8] |= 0x08;	;break;
						
		case SDLK_DOWN: keylines[0] |= 0x04;	break;
		case SDLK_o: keylines[4] |= 0x04;	break;
		case SDLK_u: keylines[5] |= 0x04;	break;
		case SDLK_r: keylines[6] |= 0x04;	break;
		case SDLK_e: keylines[7] |= 0x04;	break;
					
		case SDLK_RIGHT: keylines[0] |= 0x02;	;break;	
		case SDLK_MINUS: keylines[3] |= 0x02;	;break;	
		case SDLK_9: keylines[4] |= 0x02;	;break;	
		case SDLK_7: keylines[5] |= 0x02;	;break;	
		case SDLK_5: keylines[6] |= 0x02;	;break;	
		case SDLK_3: keylines[7] |= 0x02;	;break;	
		case SDLK_2: keylines[8] |= 0x02;	;break;
		
		case SDLK_UP: keylines[0] |= 0x01;	;break;	
		case SDLK_LEFT: keylines[1] |= 0x01;	;break;									
		case SDLK_0: keylines[4] |= 0x01;	;break;	
		case SDLK_8: keylines[5] |= 0x01;	;break;	
		case SDLK_6: keylines[6] |= 0x01;	;break;	
		case SDLK_4: keylines[7] |= 0x01;	;break;	
		case SDLK_1: keylines[8] |= 0x01;	;break;						
	}

}
