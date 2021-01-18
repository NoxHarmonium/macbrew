/*****
 * mbSerial.h
 *
 *		Public interfaces for mbSerial.h
 *
 ****/

struct SerialResponse
{
    Handle data;
    unsigned int length;
};

typedef struct SerialResponse SerialResponse;

OSErr SendCommand(char *command);
OSErr ReadResponse(SerialResponse **outResponse);
OSErr SetUpSerial();
OSErr TearDownSerial();

