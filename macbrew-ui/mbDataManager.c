#include "mbDataManager.h"
#include "mbSerial.h"

void Ping()
{
	SerialResponse *responseData;

	SetUpSerial();
	SendCommand("1 PING\r");
	ReadResponse(&responseData);
	TearDownSerial();

	DisposeResponse(&responseData);
}

void FetchBrewSessionReferences(Handle *sessionReferences)
{
	SerialResponse *responseData;

	SetUpSerial();
	SendCommand("1 LIST SESSION\r");
	ReadResponse(&responseData);
	TearDownSerial();

	DisposeResponse(&responseData);
}
