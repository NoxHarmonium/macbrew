/*****
 * mbSerial.h
 *
 *		Public interfaces for mbSerial.h
 *
 ****/

typedef struct SerialResponse
{
    Handle data;
    unsigned int length;
} SerialResponse;

OSErr SendCommand(char *command);
OSErr ReadResponse(SerialResponse **outResponse);
void DisposeResponse(SerialResponse **outResponse);
OSErr SetUpSerial(void);
OSErr TearDownSerial(void);
