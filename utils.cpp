#include <stdint.h>

char *itoa_ext(int value, unsigned int radix, unsigned int uppercase, unsigned int value_unsigned, int zero_pad)
{
	static char buffer[11];
	char *p = &buffer[10];
	int negative = 0;
	int digit;

	if (value < 0 && radix == 10 && !value_unsigned) {
		negative = 1;
		value = -value;
	}

	*p = '\0';
	do {
		p--;
		digit = value % radix;
		*p = (digit < 10 ? '0' + digit : (uppercase ? 'A' : 'a') + digit - 10);
		value /= radix;
		zero_pad--;
	} while (value > 0);

	while (zero_pad > 0) {
		p--;
		*p = '0';
		zero_pad--;
	}
	if (negative) {
		p--;
		*p = '-';
	}

	return p;
}

void memcpy(void *dst, const void *src, int len)
{
	uint8_t *s = (uint8_t *) src, *d = (uint8_t *) dst;
	for (int i = 0; i < len; i++) *d++ = *s++;
}

void memset(void *dst, uint8_t c, int len)
{
	uint8_t *d = (uint8_t *) dst;
	while (len-- > 0) {
		*d++ = c;
	}
}

int strcmp(const char *s1, const char *s2)
{
	for ( ; *s1 == *s2; s1++, s2++)
		if (*s1 == '\0')
			return 0;
	return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
}

int strncmp(const char *s1, const char *s2, int n)
{
	for ( ; n > 0; s1++, s2++, --n)
		if (*s1 != *s2)
			return ((*(unsigned char *)s1 < *(unsigned char *)s2) ? -1 : +1);
		else if (*s1 == '\0')
			return 0;
	return 0;
}
