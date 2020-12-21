; ibm_fixed.ms - fixed width IBM font
	;; BANKED:	checked, imperfect
	.area	_BASE

	.globl	font_load
	;; Perform tricks with banking to shift this font out of
	;; bank 0.  Doesnt currently work as the encoding table
	;; must always be visible.
_font_load_ibm::		; Banked
	ld	hl,#_font_ibm
	call	font_load
	ret

	; 898 bytes giving ' '-'0'-'@'-'A'-'Z'-'???'-'a'-'z'-127
_font_ibm::
	.byte	1+4		; 128 character encoding
	.byte	128-32		; Tiles required

	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0		; All map to space
	.byte	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
	.byte	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15	; 0x20
	.byte	16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
	.byte	32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47 ; 0x40
	.byte	48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63
	.byte	64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79 ; 0x60
	.byte	80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95
	
	.byte	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	.byte	0x18,0x18,0x18,0x18,0x18,0x00,0x18,0x00
	.byte	0x66,0x66,0x44,0x00,0x00,0x00,0x00,0x00
	.byte	0x00,0x24,0x7E,0x24,0x24,0x7E,0x24,0x00
	.byte	0x14,0x3E,0x55,0x3C,0x1E,0x55,0x3E,0x14
	.byte	0x62,0x66,0x0C,0x18,0x30,0x66,0x46,0x00
	.byte	0x78,0xCC,0x61,0xCE,0xCC,0xCC,0x78,0x00
	.byte	0x18,0x18,0x10,0x00,0x00,0x00,0x00,0x00
	.byte	0x04,0x08,0x18,0x18,0x18,0x18,0x08,0x04
	.byte	0x20,0x10,0x18,0x18,0x18,0x18,0x10,0x20
	.byte	0x00,0x54,0x38,0xFE,0x38,0x54,0x00,0x00
	.byte	0x00,0x18,0x18,0x7E,0x18,0x18,0x00,0x00
	.byte	0x00,0x00,0x00,0x00,0x00,0x30,0x30,0x20
	.byte	0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x00
	.byte	0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00
	.byte	0x03,0x06,0x0C,0x18,0x30,0x60,0xC0,0x00
	.byte	0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00
	.byte	0x18,0x38,0x18,0x18,0x18,0x18,0x18,0x00
	.byte	0x3C,0x66,0x0E,0x1C,0x38,0x70,0x7E,0x00
	.byte	0x7E,0x0C,0x18,0x3C,0x06,0x46,0x3C,0x00
	.byte	0x0C,0x1C,0x2C,0x4C,0x7E,0x0C,0x0C,0x00
	.byte	0x7E,0x60,0x7C,0x06,0x06,0x46,0x3C,0x00
	.byte	0x1C,0x20,0x60,0x7C,0x66,0x66,0x3C,0x00
	.byte	0x7E,0x06,0x0E,0x1C,0x18,0x18,0x18,0x00
	.byte	0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00
	.byte	0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00
	.byte	0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00
	.byte	0x00,0x18,0x18,0x00,0x18,0x18,0x10,0x00
	.byte	0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00
	.byte	0x00,0x00,0x3C,0x00,0x00,0x3C,0x00,0x00
	.byte	0x60,0x30,0x18,0x0C,0x18,0x30,0x60,0x00
	.byte	0x3C,0x46,0x06,0x0C,0x18,0x18,0x00,0x18
	.byte	0x3C,0x66,0x6E,0x6A,0x6E,0x60,0x3C,0x00
	.byte	0x3C,0x66,0x66,0x7E,0x66,0x66,0x66,0x00
	.byte	0x7C,0x66,0x66,0x7C,0x66,0x66,0x7C,0x00
	.byte	0x3C,0x62,0x60,0x60,0x60,0x62,0x3C,0x00
	.byte	0x7C,0x66,0x66,0x66,0x66,0x66,0x7C,0x00
	.byte	0x7E,0x60,0x60,0x7C,0x60,0x60,0x7E,0x00
	.byte	0x7E,0x60,0x60,0x7C,0x60,0x60,0x60,0x00
	.byte	0x3C,0x62,0x60,0x6E,0x66,0x66,0x3E,0x00
	.byte	0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00
	.byte	0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x00
	.byte	0x06,0x06,0x06,0x06,0x06,0x46,0x3C,0x00
	.byte	0x66,0x6C,0x78,0x70,0x78,0x6C,0x66,0x00
	.byte	0x60,0x60,0x60,0x60,0x60,0x60,0x7C,0x00
	.byte	0xFC,0xD6,0xD6,0xD6,0xD6,0xC6,0xC6,0x00
	.byte	0x62,0x72,0x7A,0x5E,0x4E,0x46,0x42,0x00
	.byte	0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00
	.byte	0x7C,0x66,0x66,0x7C,0x60,0x60,0x60,0x00
	.byte	0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x06
	.byte	0x7C,0x66,0x66,0x7C,0x66,0x66,0x66,0x00
	.byte	0x3C,0x62,0x70,0x3C,0x0E,0x46,0x3C,0x00
	.byte	0x7E,0x18,0x18,0x18,0x18,0x18,0x18,0x00
	.byte	0x66,0x66,0x66,0x66,0x66,0x66,0x3C,0x00
	.byte	0x66,0x66,0x66,0x66,0x66,0x64,0x78,0x00
	.byte	0xC6,0xC6,0xC6,0xD6,0xD6,0xD6,0xFC,0x00
	.byte	0x66,0x66,0x66,0x3C,0x66,0x66,0x66,0x00
	.byte	0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00
	.byte	0x7E,0x0E,0x1C,0x38,0x70,0x60,0x7E,0x00
	.byte	0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00
	.byte	0x40,0x60,0x30,0x18,0x0C,0x06,0x02,0x00
	.byte	0x78,0x18,0x18,0x18,0x18,0x18,0x78,0x00
	.byte	0x10,0x38,0x6C,0x00,0x00,0x00,0x00,0x00
	.byte	0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x00
	.byte	0x00,0xC0,0xC0,0x60,0x00,0x00,0x00,0x00
	.byte	0x00,0x3C,0x46,0x3E,0x66,0x66,0x3E,0x00
	.byte	0x60,0x7C,0x66,0x66,0x66,0x66,0x7C,0x00
	.byte	0x00,0x3C,0x62,0x60,0x60,0x62,0x3C,0x00
	.byte	0x06,0x3E,0x66,0x66,0x66,0x66,0x3E,0x00
	.byte	0x00,0x3C,0x66,0x7E,0x60,0x62,0x3C,0x00
	.byte	0x1E,0x30,0x7C,0x30,0x30,0x30,0x30,0x00
	.byte	0x00,0x3E,0x66,0x66,0x66,0x3E,0x46,0x3C
	.byte	0x60,0x7C,0x66,0x66,0x66,0x66,0x66,0x00
	.byte	0x18,0x00,0x18,0x18,0x18,0x18,0x18,0x00
	.byte	0x00,0x08,0x18,0x18,0x18,0x18,0x58,0x30
	.byte	0x60,0x64,0x68,0x70,0x78,0x6C,0x66,0x00
	.byte	0x18,0x18,0x18,0x18,0x18,0x18,0x0C,0x00
	.byte	0x00,0xFC,0xD6,0xD6,0xD6,0xD6,0xC6,0x00
	.byte	0x00,0x7C,0x66,0x66,0x66,0x66,0x66,0x00
	.byte	0x00,0x3C,0x66,0x66,0x66,0x66,0x3C,0x00
	.byte	0x00,0x7C,0x66,0x66,0x66,0x7C,0x60,0x60
	.byte	0x00,0x3E,0x66,0x66,0x66,0x66,0x3E,0x06
	.byte	0x00,0x6C,0x70,0x60,0x60,0x60,0x60,0x00
	.byte	0x00,0x3C,0x72,0x38,0x1C,0x4E,0x3C,0x00
	.byte	0x18,0x3C,0x18,0x18,0x18,0x18,0x0C,0x00
	.byte	0x00,0x66,0x66,0x66,0x66,0x66,0x3E,0x00
	.byte	0x00,0x66,0x66,0x66,0x66,0x64,0x78,0x00
	.byte	0x00,0xC6,0xC6,0xD6,0xD6,0xD6,0xFC,0x00
	.byte	0x00,0x66,0x66,0x3C,0x66,0x66,0x66,0x00
	.byte	0x00,0x66,0x66,0x66,0x26,0x1E,0x46,0x3C
	.byte	0x00,0x7E,0x0E,0x1C,0x38,0x70,0x7E,0x00
	.byte	0x0E,0x18,0x18,0x30,0x18,0x18,0x0E,0x00
	.byte	0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18
	.byte	0x70,0x18,0x18,0x0C,0x18,0x18,0x70,0x00
	.byte	0x00,0x60,0xF2,0x9E,0x0C,0x00,0x00,0x00
	.byte	0x10,0x10,0x28,0x28,0x44,0x44,0x82,0xFE
