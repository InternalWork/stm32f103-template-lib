#ifndef __UTILS_H
#define __UTILS_H

#include <io.h>

extern "C" {

char *itoa_ext(int value, unsigned int radix, unsigned int uppercase, unsigned int unsigned_value, int zero_pad);

}

template<typename OUTPUT>
void hex_dump_bytes(uint8_t *start, int count, const char *title = 0)
{
	if (title) printf<OUTPUT>("%s:\n", title);
	while (count > 0) {
		printf<OUTPUT>("%04x: %02x %02x %02x %02x\n",
			       start, start[0], start[1], start[2], start[3]);
		start += 4;
		count -= 4;
	}
}

template<typename OUTPUT>
void hex_dump_short(uint16_t *start, int count, const char *title = 0)
{
	if (title) printf<OUTPUT>("%s:\n", title);
	while (count > 0) {
		printf<OUTPUT>("%04x: %04x %04x %04x %04x\n",
			       start, start[0], start[1], start[2], start[3]);
		start += 8;
		count -= 8;
	}
}

template<typename OUTPUT>
void hex_dump_words(uint32_t *start, int count, const char *title = 0)
{
	if (title) printf<OUTPUT>("%s:\n", title);
	while (count > 0) {
		printf<OUTPUT>("%08x: %08x %08x %08x %08x\n",
			       start, start[0], start[1], start[2], start[3]);
		start += 16;
		count -= 16;
	}
}

#define ARRAY_COUNT(a) (sizeof(a) / sizeof(a[0]))
#define ABS_DIFF(a, b) (a > b ? a - b : b - a)
#define hex_dump_periph(OUTPUT, PERIPH) hex_dump_words<OUTPUT>((uint16_t *) PERIPH, sizeof(*PERIPH), #PERIPH)
#define FROM_HEX(c) (c >= '0' && c <= '9' ? c - '0' : (c >= 'A' && c <= 'F' ? c - 'A' + 10 : (c >= 'a' && c <= 'f' ? c - 'a' + 10 : 0)))

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define BIT0                   (0x0001)
#define BIT1                   (0x0002)
#define BIT2                   (0x0004)
#define BIT3                   (0x0008)
#define BIT4                   (0x0010)
#define BIT5                   (0x0020)
#define BIT6                   (0x0040)
#define BIT7                   (0x0080)
#define BIT8                   (0x0100)
#define BIT9                   (0x0200)
#define BITA                   (0x0400)
#define BITB                   (0x0800)
#define BITC                   (0x1000)
#define BITD                   (0x2000)
#define BITE                   (0x4000)
#define BITF                   (0x8000)

#endif
