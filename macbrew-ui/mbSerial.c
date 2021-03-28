#include <Serial.h>
#include "mbSerial.h"
#include "mbConstants.h"
#include "mbUtil.h"

// For some reason my serial setup echos every byte
// sent back to the receive buffer. It seems to be a problem
// with the cable or my serial/USB converter.
// If set to 1, the SendCommand function will read
// as many bytes as it sends to cancel out the echo
//
// Note: This needs to be set to zero when emulating in Basilisk II because there is no fake echo there
#define SUPRESS_ECHO 1
#define kChecksumBytes 4
// Accounts for \r\n on every response
#define kSuffixSize 2
#define kSerialBufferSize 8192

// Serial setup/teardown steps
OSErr OpenSerialDriver();
OSErr SetUpBuffer();
OSErr SetUpHandshake();
OSErr ConfigurePort();
OSErr TearDownBuffer();
OSErr CloseSerialDriver();

short VerifyChecksum(char *buffer, int length);
OSErr ReadBytes(Handle outBuffer, int count);
OSErr ReadBytesSkip(int count);
OSErr ReadLength(unsigned short *outLength);

// Output driver reference number
short gOutputRefNum;
// Input driver reference number
short gInputRefNum;
Handle gSerialBuffer;

/**
 * Serial setup/teardown steps
 */

OSErr OpenSerialDriver()
{
	OSErr result;

	// Always open output first
	result = OpenDriver("\p.AOut", &gOutputRefNum);

	if (result != noErr)
	{
		return result;
	}

	result = OpenDriver("\p.AIn", &gInputRefNum);

	return result;
}

OSErr SetUpBuffer()
{
	OSErr result;

	gSerialBuffer = NewHandle(kSerialBufferSize);
	HLock(gSerialBuffer);
	result = SerSetBuf(gInputRefNum, *gSerialBuffer, kSerialBufferSize);

	return result;
}

OSErr SetUpHandshake()
{
	OSErr result;
	SerShk serialHandshake;

	// XON/XOFF handshaking enabled
	serialHandshake.fXOn = 1;
	serialHandshake.fInX = 1;
	// Default XON/XOFF characters
	serialHandshake.xOn = 17;
	serialHandshake.xOff = 19;
	// Clear to send disabled
	serialHandshake.fCTS = 0;
	// Data terminal ready disabled
	serialHandshake.fDTR = 0;
	// Clear flags
	serialHandshake.errs = 0;
	serialHandshake.evts = 0;

	// control call 14 is better than SerHShake because it allows control of DTR
	result = SerHShake(gOutputRefNum, &serialHandshake);

	return result;
}

OSErr ConfigurePort()
{
	OSErr result;

	result = SerReset(gOutputRefNum, baud9600 + data8 + stop10 + noParity);

	return result;
}

OSErr TearDownBuffer()
{
	OSErr result;

	// Reset to default (0 means default)
	result = SerSetBuf(gInputRefNum, *gSerialBuffer, 0);

	HUnlock(gSerialBuffer);
	DisposeHandle(gSerialBuffer);

	return result;
}

OSErr CloseSerialDriver()
{
	OSErr result;

	// Terminate all pending IO
	result = KillIO(gOutputRefNum);

	// Close in specific order
	if (result != noErr)
	{
		return result;
	}

	result = CloseDriver(gInputRefNum);

	if (result != noErr)
	{
		return result;
	}

	result = CloseDriver(gOutputRefNum);

	return result;
}

/**
 * Serial interface functions
 */

short VerifyChecksum(char *buffer, int length)
{
	int checksumOffset;
	long accum, nextWord;
	int i;

	accum = 0;
	checksumOffset = length - kChecksumBytes;

	// Checksum is last 4 bytes
	accum = GetLongFromBuffer(buffer, checksumOffset);

	// Clear the checksum so that if the payload does not
	// align with 4 bytes, the remaining space will be padded with zeros
	// TODO: Why does memset freeze the machine?
	//memset(buffer + checksumOffset, 0, kChecksumBytes);
	buffer[checksumOffset] = 0;
	buffer[checksumOffset + 1] = 0;
	buffer[checksumOffset + 2] = 0;
	buffer[checksumOffset + 3] = 0;

	for (i = 0; i < checksumOffset; i += kChecksumBytes)
	{
		nextWord = GetLongFromBuffer(buffer, i);
		accum = accum ^ nextWord;
	}

	return accum == 0;
}

OSErr ReadBytes(Handle outBuffer, int count)
{
	OSErr result;
	IOParam paramBlock;

	HLock(outBuffer);

	paramBlock.ioRefNum = gInputRefNum;
	paramBlock.ioBuffer = *outBuffer;
	paramBlock.ioReqCount = count;
	paramBlock.ioCompletion = NULL;
	paramBlock.ioVRefNum = 0;
	paramBlock.ioPosMode = 0;

	result = PBRead((ParmBlkPtr)&paramBlock, FALSE);

	HUnlock(outBuffer);

	return result;
}

/**
 * Just read n bytes from the serial buffer and discard them
 */
OSErr ReadBytesSkip(int count)
{
	OSErr result;
	Handle skipBuffer = NewHandle(count);

	result = ReadBytes(skipBuffer, count);

	if (result != noErr)
	{
		return result;
	}

	DisposeHandle(skipBuffer);

	return result;
}

OSErr ReadLength(unsigned short *outLength)
{
	OSErr result;
	Handle lengthBuffer;
	int bufferSize = sizeof(unsigned short);

	lengthBuffer = NewHandle(bufferSize);

	result = ReadBytes(lengthBuffer, bufferSize);

	*outLength = GetShortFromBuffer(*lengthBuffer, 0);

	DisposeHandle(lengthBuffer);

	return result;
}

/**
 * Public functions
 */

OSErr SetUpSerial()
{
	// TODO: Is there some pattern to handle all these error code checks
	// E.g. like finally in a try catch block?
	OSErr result;

	result = OpenSerialDriver();

	if (result != noErr)
	{
		Panic("\pCould not open serial driver!");
		return result;
	}

	result = SetUpBuffer();

	if (result != noErr)
	{
		Panic("\pCould not set up serial buffer!");
		return result;
	}

	result = SetUpHandshake();

	if (result != noErr)
	{
		Panic("\pCould not set up serial handshake parameters!");
		return result;
	}

	result = ConfigurePort();

	if (result != noErr)
	{
		Panic("\pCould not set up serial port parameters!");
		return result;
	}

	return result;
}

OSErr TearDownSerial()
{
	OSErr result;

	result = TearDownBuffer();

	if (result != noErr)
	{
		Panic("\pCould not destroy serial buffer!");
		return result;
	}

	result = CloseSerialDriver();

	if (result != noErr)
	{
		Panic("\pCould not close serial driver!");
		return result;
	}

	return result;
}

OSErr SendCommand(char *command)
{
	OSErr result;
	IOParam paramBlock;
	long commandLength;

	commandLength = (long)strlen(command);

	paramBlock.ioRefNum = gOutputRefNum;
	paramBlock.ioBuffer = command;
	paramBlock.ioReqCount = commandLength;
	paramBlock.ioCompletion = NULL;
	paramBlock.ioVRefNum = 0;
	paramBlock.ioPosMode = 0;

	result = PBWrite((ParmBlkPtr)&paramBlock, FALSE);

	if (result != noErr)
	{
		return result;
	}

#if SUPRESS_ECHO
	result = ReadBytesSkip(commandLength);
#endif

	return result;
}

OSErr ReadResponse(SerialResponse **outResponse)
{
	OSErr result;
	unsigned short messageLength;
	Handle buffer;
	int csResult;
	int bufferSize;

	result = ReadLength(&messageLength);
	// Checksum and newlines are not included in the encoded length
	bufferSize = messageLength + kChecksumBytes + kSuffixSize;

	buffer = NewHandle(bufferSize);

	ReadBytes(buffer, bufferSize);
	csResult = VerifyChecksum(*buffer, messageLength + kChecksumBytes);

	if (csResult == true)
	{
		SerialResponse *response;

		response = (SerialResponse *)NewPtr(sizeof(SerialResponse));

		response->data = buffer;
		response->length = messageLength;
		*outResponse = response;
	}
	else
	{
		ShowAlert("\pChecksum validation failed!");
		DisposeHandle(buffer);
		*outResponse = 0;
	}

	return result;
}
