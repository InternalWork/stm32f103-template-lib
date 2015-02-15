#ifndef __UTILS_H
#define __UTILS_H

#include <io.h>

template<typename OUTPUT, typename DATA>
void hex_dump(DATA *start, int count, const char *title = 0)
{
	if (title) printf<OUTPUT>("%s:\n", title);
	while (count > 0) {
		printf<OUTPUT>(sizeof(DATA) == 1 ? "%04x: %02x %02x %02x %02x\n" :
			      (sizeof(DATA) == 2 ? "%04x: %04x %04x %04x %04x\n" : "%08x: %08x %08x %08x %08x\n"),
			       start, start[0], start[1], start[2], start[3]);
		start += sizeof(DATA) * 4;
		count -= sizeof(DATA) * 4;
	}
}

#define ARRAY_COUNT(a) (sizeof(a) / sizeof(a[0]))
#define ABS_DIFF(a, b) (a > b ? a - b : b - a)
#define hex_dump_periph(OUTPUT, PERIPH) hex_dump<OUTPUT, uint32_t>((uint32_t *) PERIPH, sizeof(*PERIPH), #PERIPH)
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
