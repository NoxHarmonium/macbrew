unsigned long GetLongFromBuffer(char *buffer, int offset);
unsigned short GetShortFromBuffer(char *buffer, int offset);
unsigned char GetCharFromBuffer(char *buffer, int offset);
void Panic(Str255 message);
void ShowAlert(Str255 message);
void CShowAlert(char *message);
void PascalToCStringCopy(Str255 cStringOut, const Str255 pString);
void MakeCursorBusy(void);
void MakeCursorNormal(void);
