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

typedef struct ResponseReader
{
    SerialResponse *response;
    int cursor;
} ResponseReader;

OSErr SendCommand(char *command);
OSErr ReadResponse(SerialResponse **outResponse);
void DisposeResponse(SerialResponse **outResponse);
OSErr SetUpSerial();
OSErr TearDownSerial();
