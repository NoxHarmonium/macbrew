#include <stdio.h>
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
static void ReadChar(ResponseReader *reader, char *outChar);
static void ReadUnsignedChar(ResponseReader *reader, unsigned char *outChar);
static void ReadShort(ResponseReader *reader, short *outShort);
static void ReadUnsignedShort(ResponseReader *reader, unsigned short *outShort);
static void ReadUnsignedLong(ResponseReader *reader, unsigned long *outUnsignedLong);
static void ReadString(ResponseReader *reader, StringHandle *outString);
static void ReadSequence(ResponseReader *reader, Sequence *outSequence);
static void ReadBrewSessionReference(ResponseReader *reader, Handle *outHandle);
static void ReadBrewSessionStep(ResponseReader *reader, Handle *outHandle);
static void ValidateResponse(ResponseReader *reader);
static void AssertReaderEnd(const ResponseReader *reader, const SerialResponse *responseData);
static char *BuildCommand(const char *formatString, const unsigned char *arg);

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
	char value;
	HLock(reader->response->data);
	value = GetCharFromBuffer(*reader->response->data, reader->cursor);
	reader->cursor += 1; //sizeof(Boolean);
	*outBoolean = value;
	HUnlock(reader->response->data);
}

static void ReadChar(ResponseReader *reader, char *outChar)
{
	char value;
	HLock(reader->response->data);
	value = GetCharFromBuffer(*reader->response->data, reader->cursor);
	reader->cursor += sizeof(char);
	*outChar = value;
	HUnlock(reader->response->data);
}

static void ReadUnsignedChar(ResponseReader *reader, unsigned char *outChar)
{
	unsigned char value;
	HLock(reader->response->data);
	value = GetCharFromBuffer(*reader->response->data, reader->cursor);
	reader->cursor += sizeof(unsigned char);
	*outChar = value;
	HUnlock(reader->response->data);
}

static void ReadShort(ResponseReader *reader, short *outShort)
{
	short value;
	HLock(reader->response->data);
	value = GetShortFromBuffer(*reader->response->data, reader->cursor);
	reader->cursor += sizeof(short);
	*outShort = value;
	HUnlock(reader->response->data);
}

static void ReadUnsignedShort(ResponseReader *reader, unsigned short *outShort)
{
	unsigned short value;
	HLock(reader->response->data);
	value = GetShortFromBuffer(*reader->response->data, reader->cursor);
	reader->cursor += sizeof(unsigned short);
	*outShort = value;
	HUnlock(reader->response->data);
}

static void ReadUnsignedLong(ResponseReader *reader, unsigned long *outUnsignedLong)
{
	unsigned long value;
	HLock(reader->response->data);
	value = GetLongFromBuffer(*reader->response->data, reader->cursor);
	reader->cursor += sizeof(unsigned long);
	*outUnsignedLong = value;
	HUnlock(reader->response->data);
}

static void ReadString(ResponseReader *reader, StringHandle *outString)
{
	unsigned short length = 0;
	char *buffer;
	Str255 pString;

	ReadUnsignedShort(reader, &length);

	HLock(reader->response->data);
	buffer = *(reader->response->data);

	if (length > 254)
	{
		Panic("\pCannot read strings larger than 254 characters at this time");
	}
	pString[0] = (char)length;

	memcpy(&pString[1], (char *)buffer + reader->cursor, length);

	*outString = NewString(pString);
	reader->cursor += length;

	HUnlock(reader->response->data);
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

static void ReadBrewSessionStep(ResponseReader *reader, Handle *outHandle)
{
	Handle handle = NewHandle(sizeof(BrewSessionStep));
	BrewSessionStep *brewSessionStep;
	unsigned char rawPhase;

	HLock(handle);

	//  StringHandle description;
	// short time;
	// BrewSessionStepPhase phase;

	brewSessionStep = (BrewSessionStep *)*handle;

	ReadString(reader, &brewSessionStep->description);
	ReadShort(reader, &brewSessionStep->time);
	ReadUnsignedChar(reader, &rawPhase);
	brewSessionStep->phase = rawPhase;

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

static void AssertReaderEnd(const ResponseReader *reader, const SerialResponse *responseData)
{
	Boolean success = reader->cursor == responseData->length;
	if (!success)
	{
		Panic("\pCursor is not at the end of the response. There may be an issue with the deserializer.");
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

	AssertReaderEnd(&reader, responseData);

	*outSessionReferences = sessionReference;

	DisposeResponse(&responseData);
}

void FetchBrewSession(StringHandle sessionId, BrewSessionHandle *outHandle)
{
	Str255 command;
	Str255 cSessionId;
	SerialResponse *responseData;
	ResponseReader reader;
	Handle handle = NewHandle(sizeof(BrewSession));
	BrewSession *brewSession;

	HLock(handle);

	brewSession = (BrewSession *)*handle;

	HLock((Handle)sessionId);
	PascalToCStringCopy(cSessionId, *sessionId);
	HUnlock((Handle)sessionId);

	sprintf((char *)command, "1 GET SESSION %s\r", cSessionId);

	SetUpSerial();
	SendCommand((char *)command);
	ReadResponse(&responseData);
	TearDownSerial();

	InitReader(&reader, responseData);

	ValidateResponse(&reader);

	ReadString(&reader, &brewSession->id);
	ReadString(&reader, &brewSession->phase);
	ReadString(&reader, &brewSession->batch_code);
	ReadString(&reader, &brewSession->recipe_title);
	ReadString(&reader, &brewSession->recipe_id);
	ReadString(&reader, &brewSession->style_name);
	ReadString(&reader, &brewSession->created_at);

	AssertReaderEnd(&reader, responseData);

	DisposeResponse(&responseData);

	HUnlock(handle);

	*outHandle = (BrewSessionHandle)handle;
}

static void ReadFermentationPoint(ResponseReader *reader, Handle *outHandle)
{
	Handle handle = NewHandle(sizeof(FermentationPoint));
	FermentationPoint *fermentationPointReference;

	HLock(handle);

	fermentationPointReference = (FermentationPoint *)*handle;

	ReadChar(reader, &fermentationPointReference->temp);
	ReadUnsignedChar(reader, &fermentationPointReference->gravity);

	HUnlock(handle);

	*outHandle = handle;
}

void FetchFermentationData(StringHandle sessionId, FermentationDataHandle *outHandle)
{
	Str255 command;
	Str255 cSessionId;
	SerialResponse *responseData;
	ResponseReader reader;
	Sequence *graph = (Sequence *)NewPtr(sizeof(Sequence));
	short i;
	Handle handle = NewHandle(sizeof(FermentationData));
	FermentationData *fermentationData;

	HLock(handle);

	fermentationData = (FermentationData *)*handle;

	HLock((Handle)sessionId);
	PascalToCStringCopy(cSessionId, *sessionId);
	HUnlock((Handle)sessionId);

	sprintf((char *)command, "1 GET FERMENTATION %s\r", cSessionId);

	SetUpSerial();
	SendCommand((char *)command);
	ReadResponse(&responseData);
	TearDownSerial();

	InitReader(&reader, responseData);

	ValidateResponse(&reader);

	ReadSequence(&reader, graph);
	for (i = 0; i < graph->size; i++)
	{
		ReadFermentationPoint(&reader, &graph->elements[i]);
	}
	fermentationData->graph = graph;
	ReadUnsignedLong(&reader, &fermentationData->start_date);
	ReadUnsignedLong(&reader, &fermentationData->end_date);

	AssertReaderEnd(&reader, responseData);

	DisposeResponse(&responseData);

	HUnlock(handle);

	*outHandle = (FermentationDataHandle)handle;
}

void FetchBrewSessionSteps(StringHandle sessionId, struct Sequence **outSessionSteps)
{
	Str255 command;
	Str255 cSessionId;
	SerialResponse *responseData;
	ResponseReader reader;
	Sequence *sessionSteps = (Sequence *)NewPtr(sizeof(Sequence));
	short i;

	HLock((Handle)sessionId);
	PascalToCStringCopy(cSessionId, *sessionId);
	HUnlock((Handle)sessionId);

	sprintf((char *)command, "1 LIST STEP %s\r", cSessionId);

	SetUpSerial();
	SendCommand((char *)command);
	ReadResponse(&responseData);
	TearDownSerial();

	InitReader(&reader, responseData);

	ValidateResponse(&reader);

	ReadSequence(&reader, sessionSteps);
	for (i = 0; i < sessionSteps->size; i++)
	{
		ReadBrewSessionStep(&reader, &sessionSteps->elements[i]);
	}

	AssertReaderEnd(&reader, responseData);

	*outSessionSteps = sessionSteps;

	DisposeResponse(&responseData);
}
