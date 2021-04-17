#include <string.h>

#include "mbTypes.h"
#include "mbDataManager.h"
#include "mbSerial.h"
#include "mbUtil.h"

typedef struct ResponseReader
{
	const struct SerialResponse *response;
	int cursor;
} ResponseReader;

static void InitReader(ResponseReader *reader, const SerialResponse *responseData);
static void ReadBool(ResponseReader *reader, Boolean *outBoolean);
static void ReadUnsignedShort(ResponseReader *reader, unsigned short *outShort);
static void ReadString(ResponseReader *reader, StringHandle *outString);
static void ReadSequence(ResponseReader *reader, Sequence *outSequence);
static void ReadBrewSessionReference(ResponseReader *reader, Handle *outHandle);
static void ValidateResponse(ResponseReader *reader);

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

static void InitReader(ResponseReader *reader, const SerialResponse *responseData)
{
	reader->cursor = 0;
	reader->response = responseData;
}

static void ReadBool(ResponseReader *reader, Boolean *outBoolean)
{
	char value = GetCharFromBuffer(*reader->response->data, reader->cursor);
	reader->cursor += 1; //sizeof(Boolean);
	*outBoolean = value;
}

static void ReadUnsignedShort(ResponseReader *reader, unsigned short *outShort)
{
	unsigned short value = GetShortFromBuffer(*reader->response->data, reader->cursor);
	reader->cursor += sizeof(unsigned short);
	*outShort = value;
}

static void ReadString(ResponseReader *reader, StringHandle *outString)
{
	unsigned short length = 0;
	char *buffer = *(reader->response->data);
	Str255 pString;

	ReadUnsignedShort(reader, &length);

	if (length > 254)
	{
		Panic("\pCannot read strings larger than 254 characters at this time");
	}
	pString[0] = (char)length;

	memcpy(&pString[1], (char *)buffer + reader->cursor, length);

	*outString = NewString(pString);

	reader->cursor += length;
}

static void ReadSequence(ResponseReader *reader, Sequence *outSequence)
{
	unsigned short length;
	ReadUnsignedShort(reader, &length);
	outSequence->size = length;
	// A handle of handles
	outSequence->elements = (Handle *)NewPtr(length * sizeof(Handle));
}

static void ReadBrewSessionReference(ResponseReader *reader, Handle *outHandle)
{
	Handle handle = NewHandle(sizeof(BrewSessionReference));
	BrewSessionReference *brewSessionReference;

	HLock(handle);

	brewSessionReference = (BrewSessionReference *)*handle;

	ReadString(reader, &brewSessionReference->id);
	ReadString(reader, &brewSessionReference->batch_code);
	ReadString(reader, &brewSessionReference->name);

	HUnlock(handle);

	*outHandle = handle;
}

static void ValidateResponse(ResponseReader *reader)
{
	Boolean success;
	StringHandle responseId;
	ReadString(reader, &responseId);
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

	DisposeResponse(&responseData);
}
unsigned short red;
