
#include "mbMenus.h"
#include "mbSerial.h"

extern WindowPtr mbWindow;
extern int width;

MenuHandle appleMenu, fileMenu;

enum
{
	appleID = 1,
	fileID
};

enum
{
	listSessionsItem = 1,
	quitItem = 2
};

void SetUpMenus(void)

{
	InsertMenu(appleMenu = NewMenu(appleID, "\p\024"), 0);
	AddResMenu(appleMenu, 'DRVR');
	InsertMenu(fileMenu = NewMenu(fileID, "\pFile"), 0);
	DrawMenuBar();
	AppendMenu(fileMenu, "\pList Sessions/S");
	AppendMenu(fileMenu, "\pQuit/Q");
}

void HandleMenu(long mSelect)
{
	int menuID = HiWord(mSelect);
	int menuItem = LoWord(mSelect);
	Str255 name;
	GrafPtr savePort;
	WindowPeek frontWindow;
	SerialResponse *responseData;
	char *command;

	switch (menuID)
	{
	case appleID:
		GetPort(&savePort);
		GetItem(appleMenu, menuItem, name);
		OpenDeskAcc(name);
		SetPort(savePort);
		break;

	case fileID:
		switch (menuItem)
		{
		case listSessionsItem:
			SetUpSerial();
			SendCommand("1 LIST SESSION\r");
			ReadResponse(&responseData);
			TearDownSerial();
			break;
		case quitItem:
			ExitToShell();
			break;
		}
		break;
	}
}
