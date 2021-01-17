/*****
 * mbSerial.h
 *
 *		Public interfaces for mbSerial.h
 *
 ****/

OSErr SendCommand(char* command);
OSErr ReadResponse(Handle* outBuffer);
OSErr SetUpSerial();
OSErr TearDownSerial();
