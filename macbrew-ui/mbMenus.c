
#include "mbMenus.h"
#include "mbTypes.h"
#include "mbDataManager.h"
#include "mbWSessionList.h"

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
	pingItem = 1,
	listSessionsItem = 2,
	quitItem = 3
};

void SetUpMenus(void)

{
	InsertMenu(appleMenu = NewMenu(appleID, "\p\024"), 0);
	AddResMenu(appleMenu, 'DRVR');
	InsertMenu(fileMenu = NewMenu(fileID, "\pFile"), 0);
	DrawMenuBar();
	AppendMenu(fileMenu, "\pPing/S");
	AppendMenu(fileMenu, "\pOpen Session/S");
	AppendMenu(fileMenu, "\pQuit/Q");
}

void HandleMenu(long mSelect)
{
	int menuID = HiWord(mSelect);
	int menuItem = LoWord(mSelect);
	Str255 name;
	GrafPtr savePort;
	Sequence *sessionReferences;

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
		case pingItem:
			Ping();
			break;
		case listSessionsItem:
		{
			WindowPtr sessionListWindow = SetUpSessionListWindow();
			FetchBrewSessionReferences(&sessionReferences);
			UpdateSessionListWindow(sessionListWindow, sessionReferences);

			break;
		}
		case quitItem:
			ExitToShell();
			break;
		}
		break;
	}
}
