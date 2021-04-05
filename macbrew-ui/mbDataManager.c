#include "mbTypes.h"
#include "mbDataManager.h"
#include "mbSerial.h"
#include "mbUtil.h"
#include <string.h>

// Response Structure
// -----------------
// Note: Response is in big endian to match m68k processor of the Mac
// 2 bytes message length (discarded by serial routine and stored in response struct)
// Response ID (2 bytes length (not including terminator), then n characters)
// One byte to indicate success (0 or 1)
// <PAYLOAD>
// Four bytes checksum (does not include initial 2 bytes for message length)

// Payload Structure
// -----------------
// Array: 2 bytes length, then each element packed together
// String: 2 bytes length, then n characters (no terminator)

void InitReader(ResponseReader *reader, SerialResponse *responseData)
{
	reader->cursor = 0;
	reader->response = responseData;
}

void ReadBool(ResponseReader *reader, Boolean *outBoolean)
{
	char value = GetCharFromBuffer(*reader->response->data, reader->cursor);
	reader->cursor += 1; //sizeof(Boolean);
	*outBoolean = value;
}

void ReadUnsignedShort(ResponseReader *reader, unsigned short *outShort)
{
	unsigned short value = GetShortFromBuffer(*reader->response->data, reader->cursor);
	reader->cursor += sizeof(unsigned short);
	*outShort = value;
}

void ReadString(ResponseReader *reader, unsigned char *outString)
{
	unsigned short length = 0;
	char *buffer = *(reader->response->data);

	ReadUnsignedShort(reader, &length);

	if (length > 254)
	{
		Panic("\pCannot read strings larger than 254 characters at this time");
	}
	outString[0] = (char)length;

	memcpy(outString + 1, (char *)buffer + reader->cursor, length);

	reader->cursor += length;
}

void ReadSequence(ResponseReader *reader, Sequence *outSequence)
{
	unsigned short length;
	ReadUnsignedShort(reader, &length);
	outSequence->size = length;
	// A handle of handles
	outSequence->elements = (Handle *)NewPtr(length * sizeof(Handle));
}

void ReadBrewSessionReference(ResponseReader *reader, Handle *outHandle)
{
	int s = sizeof(BrewSessionReference);
	Handle handle = NewHandle(s);
	BrewSessionReference *brewSessionReference = (BrewSessionReference *)*handle;

	ReadString(reader, brewSessionReference->id);
	ReadString(reader, brewSessionReference->batch_code);
	ReadString(reader, brewSessionReference->name);

	*outHandle = handle;
}

void ValidateResponse(ResponseReader *reader)
{
	Boolean success;
	Str255 responseId;
	ReadString(reader, responseId);
	ReadBool(reader, &success);

	// TODO: Check that response ID matches request ID
	if (!success)
	{
		// TODO: Read error message
		Panic("\pResponse from MacBrew proxy indicates an error occurred");
	}
}

void Ping()
{
	SerialResponse *responseData;

	SetUpSerial();
	SendCommand("1 PING\r");
	ReadResponse(&responseData);
	TearDownSerial();

	DisposeResponse(&responseData);
}

void FetchBrewSessionReferences(Sequence **outSessionReferences)
{

	SerialResponse *responseData;
	ResponseReader reader;
	Sequence *sessionReference = (Sequence *)NewPtr(sizeof(Sequence));
	short i;

	SetUpSerial();
	SendCommand("1 LIST SESSION\r");
	ReadResponse(&responseData);
	TearDownSerial();

	InitReader(&reader, responseData);

	ValidateResponse(&reader);

	ReadSequence(&reader, sessionReference);
	for (i = 0; i < sessionReference->size; i++)
	{
		ReadBrewSessionReference(&reader, &sessionReference->elements[i]);
	}

	*outSessionReferences = sessionReference;

	// TODO: Disposing!

	//DisposeResponse(&responseData);
}
