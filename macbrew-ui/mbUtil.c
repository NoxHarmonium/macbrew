#include <string.h>
#include "mbUtil.h"
#include "mbConstants.h"

unsigned long GetLongFromBuffer(char *buffer, int offset)
{
	unsigned char b0, b1, b2, b3;
	b0 = buffer[offset];
	b1 = buffer[offset + 1];
	b2 = buffer[offset + 2];
	b3 = buffer[offset + 3];
	return (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;
}

unsigned short GetShortFromBuffer(char *buffer, int offset)
{
	unsigned char b0, b1;
	b0 = buffer[offset];
	b1 = buffer[offset + 1];
	return (b0 << 8) | b1;
}

unsigned char GetCharFromBuffer(char *buffer, int offset)
{
	unsigned char b0;
	b0 = buffer[offset];
	return b0;
}

void Panic(Str255 message)
{
	ShowAlert(message);
	ExitToShell();
}

void ShowAlert(Str255 message)
{
	ParamText(message, "\p", "\p", "\p");
	NoteAlert(kAlertId, NULL);
}

// cppcheck-suppress unusedFunction
void CShowAlert(char *message)
{
	Str255 pString;
	strcpy((char *)pString, message);
	c2pstr((char *)pString);
	ShowAlert(pString);
}

void MakeCursorBusy(void)
{
	CursHandle x = GetCursor(watchCursor);
	SetCursor(*x);
}

void MakeCursorNormal(void)
{
	SetCursor(&arrow);
}
