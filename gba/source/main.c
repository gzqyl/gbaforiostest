//For AppStore Review "ROM Emulator" Only...

#include <gba_base.h>
#include <gba_video.h>
#include <gba_systemcalls.h>
#include <gba_interrupt.h>
// #include <xcomms.h>

#include "r6502_portfont_bin.h"
#include <string.h>

// --------------------------------------------------------------------

#define MAPADDRESS		MAP_BASE_ADR(31)	// our base map address
#define DELAY			2			// slow things down
#define TILEWIDTH		8			// how much to scroll
#define ROW				10			// what row to place text at


#define REG_KEYINPUT  (* (volatile unsigned short*) 0x4000130)

#define KEY_A        0x0001
#define KEY_B        0x0002
#define KEY_SELECT   0x0004
#define KEY_START    0x0008
#define KEY_RIGHT    0x0010
#define KEY_LEFT     0x0020
#define KEY_UP       0x0040
#define KEY_DOWN     0x0080
#define KEY_R        0x0100
#define KEY_L        0x0200

#define KEY_MASK     0xFC00

unsigned short input_cur = 0x03FF;
unsigned short input_prev = 0x03FF;

inline void key_poll()
{
    input_prev = input_cur;
    input_cur = REG_KEYINPUT | KEY_MASK;
}

inline unsigned short wasKeyPressed(unsigned short key_code)
{
    return (key_code) & (~input_cur & input_prev);
}

inline unsigned short wasKeyReleased(unsigned short key_code)
{
    return  (key_code) & (input_cur & ~input_prev);
}

inline unsigned short getKeyState(unsigned short key_code)
{
    return !(key_code & (input_cur) );
}
// --------------------------------------------------------------------

const u16 palette[] = {
	RGB8(0x40,0x80,0xc0),
	RGB8(0xFF,0xFF,0xFF),
	RGB8(0xF5,0xFF,0xFF),
	RGB8(0xDF,0xFF,0xF2),
	RGB8(0xCA,0xFF,0xE2),
	RGB8(0xB7,0xFD,0xD8),
	RGB8(0x2C,0x4F,0x8B)
};

void renderText(const u8 message[], u32 idx, u32 row)
{
	u32 i;
	u16 *temppointer;

	temppointer = (u16 *)MAPADDRESS + (row * 32);

	// write out a whole row of text to the map
	for(i=0; i<32; i++)
	{
		// check for end of message so we can wrap around properly
		if(message[idx] == 0) return;

		// write a character - we subtract 32, because the font graphics
		// start at tile 0, but our text is in ascii (starting at 32 and up)
		// in other words, tile 0 is a space in our font, but in ascii a
		// space is 32 so we must account for that difference between the two.
		*temppointer++ = message[idx++] - 32;
	}
}

const u8 logo_message[] = {
	"For App [ROM Emulator] Test"
};

const u8 logo_message2[] = {
	"simple, but enough to test"
};

const u8 start_message[] = {
	"Press Start"
};

const u8 welcome_message[] = {
	"Press other buttons to Test"
};

const u8 left_message[] = {
	"Left Pressed Just Now."
};

const u8 right_message[] = {
	"Right Pressed Just Now."
};

const u8 up_message[] = {
	"Up Pressed Just Now."
};

const u8 down_message[] = {
	"Down Pressed Just Now."
};

const u8 a_message[] = {
	"A Pressed Just Now."
};

int main() {

	irqInit();

	irqEnable(IRQ_VBLANK);

	// Allow Interrupts
	REG_IME = 1;

	u32 i, start_index;
	u16 *temppointer;

	// load the palette for the background, 7 colors
	temppointer = BG_COLORS;
	for(i=0; i<7; i++) {
		*temppointer++ = palette[i];
	}


	CpuFastSet(r6502_portfont_bin, (u16*)VRAM,(r6502_portfont_bin_size/4) | COPY32);

	// clear screen map with tile 0 ('space' tile) (256x256 halfwords)

	*((u32 *)MAP_BASE_ADR(31)) =0;
	CpuFastSet( MAP_BASE_ADR(31), MAP_BASE_ADR(31), FILL | COPY32 | (0x800/4));

	BG_OFFSET[0].x = 0; BG_OFFSET[0].y = 0;

	*((u16 *)MAPADDRESS + 1) = 0x20;	// 0x20 == '@'
	
	// set the screen base to 31 (0x600F800) and char base to 0 (0x6000000)
	BGCTRL[0] = SCREEN_BASE(31);

	// screen mode & background to display
	SetMode( MODE_0 | BG0_ON );	

	renderText(logo_message, 0, 2);

	renderText(logo_message2, 0, 4);

	renderText(start_message, 0, 8);

	start_index = 0;

	while(1) {
		VBlankIntrWait();
		key_poll();

		if ( wasKeyPressed(KEY_START) )
        {
            start_index = 1;
			renderText(welcome_message, 0, 8);

        }

		if ( start_index > 0 && wasKeyPressed(KEY_A) )
        {
            
			renderText(a_message, 0, 8);

        }

		if ( start_index > 0 && wasKeyPressed(KEY_B) )
        {
            
			start_index = 0;
			renderText(start_message, 0, 8);

        }

		if ( start_index > 0 && wasKeyPressed(KEY_LEFT) )
        {
            
			renderText(left_message, 0, 8);

        }

		if ( start_index > 0 && wasKeyPressed(KEY_RIGHT) )
        {
            
			renderText(right_message, 0, 8);

        }

		if ( start_index > 0 && wasKeyPressed(KEY_UP) )
        {
            
			renderText(up_message, 0, 8);

        }

		if ( start_index > 0 && wasKeyPressed(KEY_DOWN) )
        {
            
			renderText(down_message, 0, 8);

        }

		

	}

}
