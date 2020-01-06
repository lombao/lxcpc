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


/* This module tries to imitate the IC UM6845 	or HD6845S */



#include <stdint.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <assert.h>

 
/*
Register Index 	Register Name 	Range 	CPC Setting 	Notes
0 	Horizontal Total 			00000000 	63 	Width of the screen, in characters. Should always be 63 (64 characters). 1 character == 1μs.
1 	Horizontal Displayed 		00000000 	40 	Number of characters displayed. Once horizontal character count (HCC) matches this value, DISPTMG is set to 1.
2 	Horizontal Sync Position 	00000000 	46 	When to start the HSync signal.
3 	Horizontal and Vertical Sync Widths 	VVVVHHHH 	128+14 	HSync pulse width in characters (0 means 16 on some CRTC), should always be more than 8; VSync width in scan-lines. (0 means 16 on some CRTC. Not present on all CRTCs, fixed to 16 lines on these)
4 	Vertical Total 				x0000000 	38 	Height of the screen, in characters.
5 	Vertical Total Adjust 		xxx00000 	0 	Measured in scanlines, can be used for smooth vertical scrolling on CPC.
6 	Vertical Displayed 			x0000000 	25 	Height of displayed screen in characters. Once vertical character count (VCC) matches this value, DISPTMG is set to 1.
7 	Vertical Sync position 		x0000000 	30 	When to start the VSync signal, in characters.
8 	Interlace and Skew 			xxxxxx00 	0 	00: No interlace; 01: Interlace Sync Raster Scan Mode; 10: No Interlace; 11: Interlace Sync and Video Raster Scan Mode
9 	Maximum Raster Address 		xxx00000 	7 	Maximum scan line address on CPC can hold between 0 and 7, higher values' upper bits are ignored
10 	Cursor Start Raster 		xBP00000 	0 	Cursor not used on CPC. B = Blink On/Off; P = Blink Period Control (Slow/Fast). Sets first raster row of character that cursor is on to invert.
11 	Cursor End Raster 			xxx00000 	0 	Sets last raster row of character that cursor is on to invert
12 	Display Start Address (High)xx000000 	48
13 	Display Start Address (Low) 00000000 	0 	Allows you to offset the start of screen memory for hardware scrolling, and if using memory from address &0000 with the firmware.
14 	Cursor Address (High) 		xx000000 	0
15 	Cursor Address (Low) 		00000000 	0
16 	Light Pen Address (High) 	xx000000 		Read Only
17 	Light Pen Address (Low) 	00000000 		Read Only 
*/
/* 
The following table defines the generated memory address from the CRTC and Gate-Array signals.

Memory Address Signal 	Signal source 	Signal name
A15 	6845 	MA13
A15 	6845 	MA12
A14 	6845 	MA11
A13 	6845 	RA2
A12 	6845 	RA1
A11 	6845 	RA0
A10 	6845 	MA9
A9 	6845 	MA8
A8 	6845 	MA7
A7 	6845 	MA6
A6 	6845 	MA5
A5 	6845 	MA4
A4 	6845 	MA3
A3 	6845 	MA2
A2 	6845 	MA1
A1 	6845 	MA0
A0 	Gate-Array 	CCLK  ---> This signal is 1Mhz but video mem is at 2Mhz
*/
 
#define CRTC_NUM_REGS 18
#define CRTC_HSYNC_FREQ (( 1.0 / 16.0 ) * 1000)   /* in nano seconds, 16Mhz */

static uint8_t crtcregisters[CRTC_NUM_REGS];
static uint8_t crtc_register_selected;


/* DISPTMG signal defines the border. When DISPTMG is "1" the border colour is output to the display. */
/* static uint8_t disptmg; */


/*************************************************/
/* Externally defined functions */
extern void gate_array_hsync();

/* internally defined functions */
void crtc_init();
void crtc_set_hsync_timer();
void crtc_hsync();
void crtc_write_register(const uint8_t v);
uint8_t crtc_read_register();
void crtc_select_register(const uint8_t r);
uint16_t crtc_ram_address();
uint8_t crtc_height_in_char_lines();
uint8_t crtc_width_in_chars();
uint8_t crtc_scanlines_per_char();

/**********************************************************************/
void crtc_init() {
	crtcregisters[0] = 63;
	crtcregisters[1] = 40;
	crtcregisters[2] = 46;
	crtcregisters[3] = 128+14;
	crtcregisters[4] = 38;
	crtcregisters[5] = 0;
	crtcregisters[6] = 25;
	crtcregisters[7] = 30;
	crtcregisters[8] = 0;
	crtcregisters[9] = 7;
	crtcregisters[10] = 0;
	crtcregisters[11] = 0;
	crtcregisters[12] = 48;
	crtcregisters[13] = 0;
	crtcregisters[14] = 0;
	crtcregisters[15] = 0;
	crtcregisters[16] = 0;
	crtcregisters[17] = 0;
	
	crtc_set_hsync_timer();
	
}

/**********************************************************************/
/* this the base video ram address */
uint16_t crtc_ram_address() {
	return ( (uint16_t)(crtcregisters[12] << 8) + crtcregisters[13]);
	
	
	/* caculate the video ram base */
	uint16_t base = 0x0000;
	switch(( crtcregisters[12] & 0x30) >> 4) {
		case 0: base = 0x0000; break;
		case 1: base = 0x4000; break;
		case 2: base = 0x8000; break;
		case 3: base = 0xC000; break;
	}
	
	/* 32Kb buffer is not supported */
	assert(  ((crtcregisters[12] & 0x0C) >> 2) == 3 );
		
    base += (uint16_t)((crtcregisters[12] & 0x03) << 8) + crtcregisters[13]; 
	
	return base;
	
}

uint8_t crtc_height_in_char_lines() {
	return crtcregisters[6];
}

uint8_t crtc_width_in_chars() {
	return crtcregisters[1];
}

uint8_t crtc_scanlines_per_char() {
	return crtcregisters[9];
}

/*************************************************/
void crtc_select_register(const uint8_t r) {
	crtc_register_selected = r;
}


/* note , the IC we emulate only allows read to regs between 12 and 17 */
uint8_t crtc_read_register() {
	if ( crtc_register_selected >= 18 ) {
		return 0;
	}
	if ( crtc_register_selected >= 12 ) {
		return crtcregisters[ crtc_register_selected ];
	}
	return 0x0;
}

/* note, the IC we emulate only allows to write up to the register 15 */
void crtc_write_register(const uint8_t v) {
	if ( crtc_register_selected <= 15 ) {
		crtcregisters[ crtc_register_selected ] = v;
	}
}

/****************************************************/

void crtc_hsync() {
	gate_array_hsync();
}

/* this should be called once to initialize the hsync timer */
void crtc_set_hsync_timer() {
	
	struct sigaction act;
	clockid_t clock_id;
	timer_t timer_id;
	struct sigevent clock_sig_event;
	struct itimerspec timer_value;
	int ret;

	/* Register new action for SIGUSR1 */
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler =  crtc_hsync; 			/* this is the handler 				*/
	ret = sigaction(SIGUSR1, &act, NULL);	/* we use SIGUSR1 for hsync 	 	*/
	/* assert(ret == 0); */

	clock_id = CLOCK_REALTIME;
	memset(&clock_sig_event, 0, sizeof( struct sigevent));
	clock_sig_event.sigev_notify = SIGEV_SIGNAL;
	clock_sig_event.sigev_signo = SIGUSR1;
	clock_sig_event.sigev_notify_attributes = NULL;
	
	/* Creating process interval timer */
	ret = timer_create(clock_id, &clock_sig_event, &timer_id);
	/* assert(ret == 0); */

	/* setitng timer interval values */
	memset(&timer_value, 0, sizeof(struct itimerspec));
	timer_value.it_interval.tv_sec = 0;
	timer_value.it_interval.tv_nsec = CRTC_HSYNC_FREQ;

	/* setting timer initial expiration values*/
	timer_value.it_value.tv_sec = 0;
	timer_value.it_value.tv_nsec = CRTC_HSYNC_FREQ;

	/* Create timer */
	ret = timer_settime(timer_id, 0, &timer_value, NULL);
	assert(ret == 0);

     
        /*
         *
         * We can also use SIGEV_THREAD instead of SIGEV_SIGNAL in clock_sig_event.sigev_notify
         * while creating timer_id using timer_create().In that case we need to check for other parameter
         * as well.
         */

}

