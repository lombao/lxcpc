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

/*************************************************/
/* Externally defined functions */
extern uint16_t crtc_ram_address();
extern uint8_t crtc_height_in_char_lines();
extern uint8_t crtc_width_in_chars();
extern uint8_t crtc_scanlines_per_char();
extern int gate_array_screen_mode();
extern uint8_t access_ram_video(uint16_t addr);
extern int gate_array_pen_color(uint8_t pen);

/* internally defined */
void video_createframe();
void window_drawpixel(const int x, const int y,const int color);
void window_destroy();
void window_init();

/**********************************************************************/ 


static SDL_Renderer * renderer;	
static SDL_Window  	* window;

/*
#define NUM_VIDEO_MODES 4
static struct {
	uint16_t x;
	uint16_t y; 	
} video_mode_res[NUM_VIDEO_MODES] = {
	{ 160,200},
	{ 320,200},
	{ 640,200},
	{ 160,200}
};
*/

#define NUM_COLORS 32
static struct {
	int r;
	int g;
	int b;
} colors[NUM_COLORS] = {
	{ 50,50,50 }, /* white */
	{ 50,50,50 }, /* white */
	{ 0,100,50 }, /* sea green */
	{ 100,100,50}, /* pastel yellow */
	{ 0,0,50 }, /* blue */
	{ 100,0,50 }, /* purple */
	{ 0,50,50 }, /* cyan */
	{ 100,50,50 }, /* pink */
	{ 100,0,50}, /* purple */
	{ 100,100,50 }, /* pastel yellow */
	{ 100,100,0},  /* bright yellow */
	{ 100,100,100}, /* bright white */
	{ 100,0,0}, /* bright red */
	{ 100,0,100}, /* bright magenta */
	{ 100,50,0}, /* orange */
	{ 100,50,100}, /* pastel magenta */
	{ 0,0,50}, /* blue */
	{ 0,100,50 }, /* sea green */
	{ 0,100,0 }, /* bright green */
	{ 0,100,100 }, /* bright cyan */
	{ 0,0,0 }, /* black */
	{ 0,0,100 }, /* bright blue */
	{ 0,50,0 }, /* green */
	{ 0,50,100 }, /* sky blue */
	{ 50,0,50}, /*  magenta */
	{ 50,100,50}, /* pastel green */
	{ 50,100,0}, /* lima */
	{ 50,100,100}, /* pastel cyan */
	{ 50,0,0}, /* redd */
	{ 50,0,100}, /* mauve */
	{ 50,50,0}, /* yellow */
	{ 50,50,100}, /* pastel blue */	
};



/**********************************************************************/
void window_destroy() {
	SDL_DestroyRenderer(renderer);
	renderer = NULL;
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();
}

void window_init() {
	/* initialize SDL */
	if(SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER)!=0) {
		fprintf(stderr,"SDL error -> %sn", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);
	
	window = SDL_CreateWindow(
			"Lx6128",SDL_WINDOWPOS_UNDEFINED , SDL_WINDOWPOS_UNDEFINED,
			640, 400, 0
	);
	
	renderer = SDL_CreateRenderer(  window, -1,
			SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC
	);
    
	/* warning , we set the background as blue, hardcoded */
	SDL_SetRenderDrawColor(renderer,0,0,255,SDL_ALPHA_OPAQUE);	
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
}

void window_drawpixel(const int x, const int y,const int color) {
		
	SDL_SetRenderDrawColor(renderer,colors[color].r,colors[color].g,colors[color].b,SDL_ALPHA_OPAQUE);	
	SDL_RenderDrawPoint(renderer,x,y);						
	
}


/**********************************************************************/ 
/**********************************************************************/ 
void video_createframe() {
	
	uint16_t base = crtc_ram_address();
	
	/* uint8_t width_in_chars			= crtc_width_in_chars(); */
	uint8_t scanlines_per_char 		= crtc_scanlines_per_char() + 1;
	uint8_t height_in_char_lines	= crtc_height_in_char_lines();
	
	
	uint8_t screen_mode				= gate_array_screen_mode();
	
	int x,y;
	uint8_t b,p;
	
	for( int l=0; l<height_in_char_lines; l++) {
		uint8_t addr = base + ( 0x50 * l );
		for ( int c=0; c < 0x50; c++ ) {
			addr += c;		
			for( int s=0; s < scanlines_per_char; s++ ) {
				addr += (0x800*s);
				b = access_ram_video(addr);
				y = (scanlines_per_char * l)+s; 
				switch (screen_mode) {
					case 0: x = c * 2;
							p = ((b & 0x80)>>7) | ((b&0x08)>>2) | ((b&0x20)>>3) | ((b&0x02) << 2) ;
							window_drawpixel(x,y,gate_array_pen_color(p));
							x = (c * 2) + 1;
							p = ((b & 0x40)>>6) | ((b&0x04)>>1) | ((b&0x10)>>2) | ((b&0x01) << 3) ;
							window_drawpixel(x,y,gate_array_pen_color(p));	
							break;
							
					case 1: x = c * 4;
							p = ((b & 0x80)>>6) | ((b&0x08)>>3);
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							x = (c * 4) + 1;
							p = ((b & 0x40)>>5) | ((b&0x04)>>2);
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							x = (c * 4) + 2;
							p = ((b & 0x20)>>4) | ((b&0x02)>>1);
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							x = (c * 4) + 3;
							p = ((b & 0x10)>>3) | ((b&0x01));
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							break;
							
					case 2: x = c * 8;
							p = b >> 7;
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							x = (c * 8) + 1;
							p = (b >> 6) & 0x01;
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							x = (c * 8) + 2;
							p = (b >> 5) & 0x01;
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							x = (c * 8) + 3;
							p = (b >> 4) & 0x01;
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							x = (c * 8) + 4;
							p = (b >> 3) & 0x01;
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							x = (c * 8) + 5;
							p = (b >> 2) & 0x01;							
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							x = (c * 8) + 6;
							p = (b >> 2) & 0x01;
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							x = (c * 8) + 7;
							p = b & 0x01;
							window_drawpixel(x,y,gate_array_pen_color(p));
							
							break;
					case 3: break;
				}
			}
		}
	} 
	SDL_RenderPresent(renderer);

}



 
