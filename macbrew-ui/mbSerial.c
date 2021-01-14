#include <serial.h>
#include "mbSerial.h"
#include "mbConstants.h"

// For some reason my serial setup echos every byte
// sent back to the receive buffer. It seems to be a problem
// with the cable or my serial/USB converter.
// If set to 1, the SendCommand function will read 
// as many bytes as it sends to cancel out the echo
#define SUPRESS_ECHO 1

short VerifyChecksum(char* buffer, int length);
OSErr ReadBytes(Ptr outBuffer, int count);
OSErr ReadBytesSkip(int count);
OSErr ReadLength(unsigned int* outLength);
void ShowAlert(char* message);

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

	// Always open output first
	result = OpenDriver("\p.AOut", &gOutputRefNum);
	
	if (result != noErr) {
		return result;
	}
	
	result = OpenDriver("\p.AIn", &gInputRefNum);
	
	if (result != noErr) {
		return result;
	}
	
	result = SerReset(gOutputRefNum, baud9600 + data8 + stop10 + noParity);
	
	if (result != noErr) {
		return result;
	}
	
	gSerialBuffer = NewHandle(32 * 1024);
	HLock(gSerialBuffer);
	result = SerSetBuf(gInputRefNum, *gSerialBuffer, 32 * 1024);
	
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
	long accum;
	int i;

	checksumOffset = length - 4;
	// Checksum is last 4 bytes
	accum = *((unsigned long*)buffer[checksumOffset]);
	
	// Çlear the checksum so that if the payload does not
	// align with 4 bytes, the remaining space will be padded with zeros
	memset(buffer, checksumOffset, 4);
	
	for (i = 0; i < checksumOffset; i += 4) {
		accum = accum ^ *((unsigned long*)buffer[i]);
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
	Ptr skipBuffer;
	int comp;
	char temp[20];
	
	skipBuffer = NewPtrClear(count);
	result = ReadBytes(skipBuffer, count + 1);
	// TODO: Remove this
	comp = strcmp(result, "1 LIST SESSION\r");
	
	//sprintf(temp, "result %d");
	//ShowAlert(temp);
	ShowAlert((char*)skipBuffer);
	
	DisposePtr(skipBuffer);
	return result;
}



void ShowAlert(char* message) {
	int myAlertItem;
	ParamText(message, "", "", "");
	myAlertItem = NoteAlert(kAlertId, NULL);
}

OSErr ReadLength(unsigned int* outLength) {
	OSErr result;
	Ptr lengthBuffer;
	unsigned int messageLength;
	long tempBufferLength;
	unsigned char a;
	unsigned char b;
	unsigned char c;
	unsigned char d;
	
	SerGetBuf(gInputRefNum, &tempBufferLength);
	
	lengthBuffer = NewPtrClear(tempBufferLength + 1);
	result = ReadBytes(lengthBuffer, tempBufferLength);
	
	if (result != noErr) {
		return result;
	}
	
	// Read in 16 bit BE integer;
	a = *lengthBuffer; 
	b = *(lengthBuffer+1);
	c = *(lengthBuffer+2);
	d = *(lengthBuffer+3);
	
	
	DisposePtr(lengthBuffer);
	
	return noErr;
}

OSErr ReadResponse(char** outBuffer) {
	// TODO: Is there some pattern to handle all these error code checks
	// E.g. like finally in a try catch block?
	OSErr result;
	unsigned int messageLength;
	
	result = ReadLength(&messageLength);
	
	return result;
}
