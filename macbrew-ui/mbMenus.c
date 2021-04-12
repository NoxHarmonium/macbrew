
#include "mbDataManager.h"
#include "mbMenus.h"
#include "mbTypes.h"
#include "mbDSessionList.h"
#include "mbUtil.h"

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
	AppendMenu(fileMenu, "\pOpen Session/O");
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
		{
			Ping();
			break;
		}
		case listSessionsItem:
		{
			WindowPtr sessionListDialog = SessionListDialogSetUp();
			MakeCursorBusy();
			FetchBrewSessionReferences(&sessionReferences);
			MakeCursorNormal();
			SessionListDialogSetSessions(sessionListDialog, sessionReferences);
			SessionListDialogShow(sessionListDialog);
			SessionListDialogDestroy(sessionListDialog);
			break;
		}
		case quitItem:
			ExitToShell();
			break;
		}
		break;
	}
}
