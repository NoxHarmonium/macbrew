/*****
 * mbSerial.h
 *
 *		Public interfaces for mbSerial.h
 *
 ****/

OSErr SendCommand(char* command);
OSErr ReadResponse(char** outBuffer);
OSErr SetUpSerial();
OSErr TearDownSerial();
