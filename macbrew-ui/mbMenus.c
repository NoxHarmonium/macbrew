
#include "mbDataManager.h"
#include "mbMenus.h"
#include "mbTypes.h"
#include "mbDSessionList.h"
#include "mbUtil.h"
#include "mbWViewSession.h"
#include "mbWViewSteps.h"

MenuHandle appleMenu, fileMenu, sessionMenu;
// TODO: Support multiple sessions
// TODO: Clear when closing session window
BrewSessionReferenceHandle selectedSession = NULL;

enum
{
	appleID = 1,
	fileID,
	sessionID
};

enum
{
	pingItem = 1,
	listSessionsItem = 2,
	quitItem = 3
};

enum
{
	stepsItem = 1
};

void SetUpMenus(void)
{
	InsertMenu(appleMenu = NewMenu(appleID, "\p\024"), 0);
	AddResMenu(appleMenu, 'DRVR');

	InsertMenu(fileMenu = NewMenu(fileID, "\pFile"), 0);
	AppendMenu(fileMenu, "\pPing/P");
	AppendMenu(fileMenu, "\pOpen Session/O");
	AppendMenu(fileMenu, "\pQuit/Q");

	InsertMenu(sessionMenu = NewMenu(sessionID, "\pSession"), 0);
	AppendMenu(sessionMenu, "\pSteps/S");

	DrawMenuBar();
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
			short selectedItem;
			FermentationDataHandle fermentationData;
			BrewSessionHandle brewSession;
			WindowPtr viewSessionWindow;
			MakeCursorBusy();
			FetchBrewSessionReferences(&sessionReferences);
			MakeCursorNormal();
			SessionListDialogSetSessions(sessionListDialog, sessionReferences);
			selectedItem = SessionListDialogShow(sessionListDialog);
			if (selectedItem < 0 || selectedItem > sessionReferences->size - 1)
			{
				Panic("\pSelected item out of range!");
			}
			SessionListDialogDestroy(sessionListDialog);
			selectedSession = (BrewSessionReferenceHandle)sessionReferences->elements[selectedItem];
			HLock((Handle)selectedSession);
			FetchBrewSession((*selectedSession)->id, &brewSession);
			HUnlock((Handle)selectedSession);

			viewSessionWindow = SessionViewWindowSetUp();
			SessionViewSetSession(viewSessionWindow, brewSession);

			FetchFermentationData((*selectedSession)->id, &fermentationData);
			SessionViewSetFermentationData(viewSessionWindow, fermentationData);

			// TODO: We own fermentationData and  brewSession so we should probably clean them up
			// 		 but we don't know when the window is destroyed so should we get
			//       transfer ownership to the window and let it clean it up?
			break;
		}
		case quitItem:
			ExitToShell();
			break;
		}
		break;
	case sessionID:
		if (selectedSession == NULL)
		{
			// Can't open steps if no session selected
			return;
		}
		switch (menuItem)
		{
		case stepsItem:
		{
			WindowPtr viewStepsWindow;
			Sequence *sessionStepsHandle;

			viewStepsWindow = StepsViewWindowSetUp();

			FetchBrewSessionSteps((*selectedSession)->id, &sessionStepsHandle);
			StepsViewSetSteps(viewStepsWindow, sessionStepsHandle);

			// TODO: We own sessionStepsHandle so we should probably clean it up
			// 		 but we don't know when the window is destroyed so should we get
			//       transfer ownership to the window and let it clean it up?
			break;
		}
		break;
		}
	}
}
