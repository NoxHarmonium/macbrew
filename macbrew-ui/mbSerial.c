#include <serial.h>
#include "mbSerial.h"
#include "mbConstants.h"
#include "mbUtil.h"

// For some reason my serial setup echos every byte
// sent back to the receive buffer. It seems to be a problem
// with the cable or my serial/USB converter.
// If set to 1, the SendCommand function will read 
// as many bytes as it sends to cancel out the echo
#define SUPRESS_ECHO 1
#define kChecksumBytes 4
// Accounts for \r\n on every response
#define kSuffixSize 2

short VerifyChecksum(char* buffer, int length);
OSErr ReadBytes(Ptr outBuffer, int count);
OSErr ReadBytesSkip(int count);
OSErr ReadLength(unsigned int* outLength);
void ShowAlert(Str255 message);

// Output driver reference number
int gOutputRefNum;
// Input driver reference number
int gInputRefNum;
Handle gSerialBuffer;

OSErr TearDownSerial() {
	OSErr result;
	
	// Reset to default (0 means default)
	result = SerSetBuf(gInputRefNum, *gSerialBuffer, 0);
	
	HUnlock(gSerialBuffer);
	DisposeHandle(gSerialBuffer);
	
	if (result != noErr) {
		return result;
	}

	// Terminate all pending IO
	result = KillIO(gOutputRefNum);
	
	// Close in specific order
	if (result == noErr) {
		result = CloseDriver(gInputRefNum);
	}
	if (result == noErr) {
		result = CloseDriver(gOutputRefNum);
	}
	
	return result;
}

OSErr SetUpSerial() {
	OSErr result;
	SerShk serialHandshake;

	// Always open output first
	result = OpenDriver("\p.AOut", &gOutputRefNum);
	
	if (result != noErr) {
		return result;
	}
	
	result = OpenDriver("\p.AIn", &gInputRefNum);
	
	if (result != noErr) {
		return result;
	}
	
	gSerialBuffer = NewHandle(8 * 1024);
	HLock(gSerialBuffer);
	result = SerSetBuf(gInputRefNum, *gSerialBuffer, 8 * 1024);
	
	if (result != noErr) {
		return result;
	}
	
	serialHandshake.fXOn = 1;
	serialHandshake.xOn = 17;
	serialHandshake.xOff = 19;
	serialHandshake.fCTS = 0;
	serialHandshake.errs = 0;
	serialHandshake.evts = 0;
	serialHandshake.fInX = 1;
	
	// control call 14 is better than SerHShake because it allows control of DTR
	result = SerHShake(gOutputRefNum, &serialHandshake);
	
	if (result != noErr) {
		return result;
	}
	
	result = SerReset(gOutputRefNum, baud9600 + data8 + stop10 + noParity);
	
	return result;
}

OSErr SendCommand(char* command) {
	// TODO: Is there some pattern to handle all these error code checks
	// E.g. like finally in a try catch block?
	OSErr result;
	IOParam paramBlock;
	unsigned int commandLength;

	commandLength = strlen(command);
	
	paramBlock.ioRefNum = gOutputRefNum;
	paramBlock.ioBuffer = command;
	paramBlock.ioReqCount = commandLength;
	paramBlock.ioCompletion = 0;
	paramBlock.ioVRefNum = 0;
	paramBlock.ioPosMode = 0;

	result = PBWrite((ParmBlkPtr)&paramBlock, FALSE);
	
	if (result != noErr) {
		return result;
	}
	
#if SUPRESS_ECHO
	result = ReadBytesSkip(commandLength);
#endif
	
	return result;
}


short VerifyChecksum(char* buffer, int length) {
	int checksumOffset;
	long accum, nextWord;
	int i;
	Str255 temp;

	accum = 0;
	checksumOffset = length - kChecksumBytes;

	// Checksum is last 4 bytes
	accum = GetLongFromBuffer(buffer, checksumOffset);
	
	// Çlear the checksum so that if the payload does not
	// align with 4 bytes, the remaining space will be padded with zeros
	// TODO: Why does memset freeze the machine?
	//memset(buffer + checksumOffset, 0, kChecksumBytes);
	buffer[checksumOffset] = 0;
	buffer[checksumOffset+1] = 0;
	buffer[checksumOffset+2] = 0;
	buffer[checksumOffset+3] = 0;
	
	for (i = 0; i < checksumOffset; i += kChecksumBytes) {
		nextWord = GetLongFromBuffer(buffer, i);
		//sprintf((char*)temp, "%lx", nextWord);
		//c2pstr((char*)temp);
		//ShowAlert(temp);
		accum = accum ^ nextWord;
	}
	
	return accum == 0;
}

OSErr ReadBytes(Ptr outBuffer, int count) {
	OSErr result;
	IOParam paramBlock;

	paramBlock.ioRefNum = gInputRefNum;
	paramBlock.ioBuffer = outBuffer;
	paramBlock.ioReqCount = count;
	paramBlock.ioCompletion = NULL;
	paramBlock.ioVRefNum = 0;
	paramBlock.ioPosMode = 0;

	result = PBRead((ParmBlkPtr)&paramBlock, FALSE);
	
	return result;
}

/**
 * Just read n bytes from the serial buffer and discard them
 */
OSErr ReadBytesSkip(int count) {
	OSErr result;
	Handle skipBuffer = NewHandle(count);
	
	HLock(skipBuffer);
	result = ReadBytes((char*)*skipBuffer, count);
	HUnlock(skipBuffer);
	
	if (result != noErr) {
		return result;
	}
	
	DisposeHandle(skipBuffer);
	
	return result;
}



void ShowAlert(Str255 message) {
	int myAlertItem;
	
	ParamText(message, "", "", "");
	myAlertItem = NoteAlert(kAlertId, NULL);
}

OSErr ReadLength(unsigned int* outLength) {
	OSErr result;
	
	result = ReadBytes((char*)outLength, sizeof(unsigned int));
	
	return result;
}

OSErr ReadResponse(Handle* outBuffer) {
	// TODO: Is there some pattern to handle all these error code checks
	// E.g. like finally in a try catch block?
	OSErr result;
	unsigned int messageLength;
	Handle buffer;
	Str255 temp;
	int i;
	int csResult;
	int bufferSize;
	
	result = ReadLength(&messageLength);
	bufferSize = messageLength + kChecksumBytes + kSuffixSize;
	
	buffer = NewHandleClear(bufferSize);
	
	HLock(buffer);
	ReadBytes(*buffer, bufferSize);
	csResult = VerifyChecksum(*buffer, messageLength + kChecksumBytes);
	HUnlock(buffer);
	
	if (csResult == true) {
		*outBuffer = buffer;
	} else {
		DisposeHandle(buffer);
		*outBuffer = 0;
	}
	
	return result;
}
