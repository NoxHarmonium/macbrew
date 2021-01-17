#include "mbUtil.h"

long GetLongFromBuffer(char* buffer, int offset) {
	char b0, b1, b2, b3;
	b0 = buffer[offset];
	b1 = buffer[offset + 1];
	b2 = buffer[offset + 2];
	b3 = buffer[offset + 3];
	return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}
