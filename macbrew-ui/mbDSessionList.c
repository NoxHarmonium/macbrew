#include <string.h>
#include <Events.h>
#include "mbConstants.h"
#include "mbUtil.h"
#include "mbDSessionList.h"
#include "mbTypes.h"
#include "mbListUtils.h"
#include "mbDialogUtils.h"

typedef struct SessionListDialogState
{
	ListHandle listHandle;
	ListItem **sessionListItems;
	short sessionListItemCount;
} SessionListDialogState;

pascal Boolean SessionListEventFilterProc(DialogPtr theDialog, EventRecord *theEvent, short *itemHit);

static void SessionListDialogInitState(DialogPtr theDialog);
static SessionListDialogState *SessionListDialogLockState(DialogPtr theDialog);
static void *SessionListDialogUnlockState(DialogPtr theDialog);

static void SetUpSessionListControl(DialogPtr parentDialog);
static void DestroySessionListControl(DialogPtr parentDialog);
static ListRec *SessionListControlLock(const SessionListDialogState *dialogState);
static void SessionListControlUnlock(const SessionListDialogState *dialogState);
Cell SessionListGetSelectedCell(DialogPtr theDialog);
static void SessionListControlHandleKeyboard(DialogPtr theDialog, char key);
static void SessionListControlUpdate(DialogPtr theDialog);
static Boolean SessionListControlMouseDown(DialogPtr theDialog, EventRecord theEvent);

static void SetUpButtons(DialogPtr parentDialog);
static void DestroyButtons(DialogPtr parentDialog);

pascal Boolean SessionListEventFilterProc(DialogPtr theDialog, EventRecord *theEvent, short *itemHit)
{

	if (theEvent->what == updateEvt)
	{
		// TODO: Filter for what the update is about?
		SessionListControlUpdate(theDialog);
		return FALSE;
	}
	if (theEvent->what == mouseDown)
	{
		WindowPtr theWindow;
		int windowCode = FindWindow(theEvent->where, &theWindow);
		short windowKind = 0;

		if (theWindow != NULL)
		{
			windowKind = ((WindowPeek)theWindow)->windowKind;
		}
		if (windowCode == inContent && windowKind == kDialogWindowKind && SessionListControlMouseDown(theDialog, *theEvent))
		{
			*itemHit = ok;
			return TRUE;
		}
	}
	else
	{
		switch (theEvent->what)
		{
		case keyDown:
		case autoKey:
		{
			char key = theEvent->message & charCodeMask;
			if (key == kUpArrow || key == kDownArrow)
			{
				SessionListControlHandleKeyboard(theDialog, key);
			}
			else if (key == kReturnCharCode || key == kEnterCharCode)
			{
				*itemHit = ok;
				return TRUE;
			}
			else if (key == kEscapeCharCode ||
					 ((theEvent->modifiers & cmdKey) > 0 && key == kPeriodCharCode))
			{
				*itemHit = cancel;
				return TRUE;
			}
		}
		}
	}
	return FALSE;
}

static void SetUpSessionListControl(DialogPtr parentDialog)
{
	// List Manager functions
	// See: http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/MoreToolbox/MoreToolbox-210.html#HEADING210-0

	// Constants
	const Boolean doDraw = TRUE;		   // always draw list after changes
	const Boolean noGrow = TRUE;		   // don't leave room for size box
	const Boolean includeScrollBar = TRUE; // leave room for scroll bar
	const short scrollBarWidth = 15;	   // width of vertical scroll bar
	const short columns = 1;			   // number of columns in the list
	const short padding = 20;			   // padding around the list view

	// Variables
	SessionListDialogState *dialogState = SessionListDialogLockState(parentDialog);
	Rect dataBounds;
	Point cellSize;
	Rect visibleRect;

	visibleRect = parentDialog->portRect;
	visibleRect.top = visibleRect.top + padding;
	visibleRect.left = visibleRect.left + padding;
	visibleRect.right = visibleRect.right - scrollBarWidth - padding;
	visibleRect.bottom = visibleRect.bottom - (padding * 2);

	// start with a list that contains no rows
	SetRect(&dataBounds, 0, 0, columns, 0);

	// let the List Manager calculate the size of a cell}
	SetPt(&cellSize, 0, 0);

	// Create the list
	dialogState->listHandle = LNew(&visibleRect, &dataBounds, cellSize, 0, parentDialog, doDraw, noGrow, !includeScrollBar, includeScrollBar);
	dialogState->sessionListItemCount = 0;

	SessionListDialogUnlockState(parentDialog);
}

static void DestroySessionListControl(DialogPtr parentDialog)
{
	short i;
	SessionListDialogState *dialogState = SessionListDialogLockState(parentDialog);

	for (i = 0; i < dialogState->sessionListItemCount; i++)
	{
		ListItem *item = dialogState->sessionListItems[i];
		DestroyListItem(item);
	}
	dialogState->sessionListItemCount = 0;

	if (dialogState->listHandle != NULL)
	{
		LDispose(dialogState->listHandle);
		dialogState->listHandle = NULL;
	}

	SessionListDialogUnlockState(parentDialog);
}

static void SessionListDialogInitState(DialogPtr theDialog)
{
	const Handle sessionListWindowStateHandle = NewHandleClear(sizeof(SessionListDialogState));
	SetWRefCon(theDialog, (long)sessionListWindowStateHandle);
}

static SessionListDialogState *SessionListDialogLockState(DialogPtr theDialog)
{
	const Handle sessionListWindowStateHandle = (Handle)GetWRefCon(theDialog);

	HLock(sessionListWindowStateHandle);
	return (SessionListDialogState *)*sessionListWindowStateHandle;
}

static void *SessionListDialogUnlockState(DialogPtr theDialog)
{
	const Handle sessionListWindowStateHandle = (Handle)GetWRefCon(theDialog);

	HUnlock(sessionListWindowStateHandle);
}

static ListRec *SessionListControlLock(const SessionListDialogState *dialogState)
{
	if (dialogState->listHandle == NULL)
	{
		Panic("\pSession list window not set up. Please call SessionListDialogSetUp()");
	}

	HLock((Handle)dialogState->listHandle);
	return *dialogState->listHandle;
}

static void SessionListControlUnlock(const SessionListDialogState *dialogState)
{
	if (dialogState->listHandle == NULL)
	{
		Panic("\pSession list window not set up. Please call SessionListDialogSetUp()");
	}

	HUnlock((Handle)dialogState->listHandle);
}

DialogPtr SessionListDialogSetUp(void)
{
	// Parent Window
	DialogPtr sessionListDialog = NULL;

	sessionListDialog = GetNewDialog(kSessionListDialog, sessionListDialog, (WindowPtr)-1L);

	SessionListDialogInitState(sessionListDialog);

	// Controls
	SetupDialogButtonOutline(sessionListDialog, kSessionListUserItem);
	SetUpSessionListControl(sessionListDialog);

	return sessionListDialog;
}

void SessionListDialogDestroy(DialogPtr theDialog)
{
	// Clean up item data
	short i;
	SessionListDialogState *dialogState = SessionListDialogLockState(theDialog);

	for (i = 0; i < dialogState->sessionListItemCount; i++)
	{
		DisposePtr((Ptr)dialogState->sessionListItems[i]);
	}

	SessionListDialogUnlockState(theDialog);

	// Controls
	DestroySessionListControl(theDialog);

	// Parent Window
	if (theDialog != NULL)
	{
		DisposeDialog(theDialog);
	}
}

void SessionListDialogSetSessions(DialogPtr theDialog, Sequence *sessionReferences)
{
	short i;
	SessionListDialogState *dialogState = SessionListDialogLockState(theDialog);

	if (dialogState->sessionListItemCount > 0)
	{
		// TODO: Delete existing rows
		Panic("\pCan only add rows to an empty list at this time");
	}

	dialogState->sessionListItems = (ListItem **)NewPtr(sessionReferences->size * sizeof(ListItem *));
	dialogState->sessionListItemCount = sessionReferences->size;

	for (i = 0; i < sessionReferences->size; i++)
	{
		Handle sessionHandle = sessionReferences->elements[i];
		BrewSessionReference *brewSession;
		ListItem *item;
		Cell cell;

		SetPt(&cell, 0, i);
		HLock(sessionHandle);
		brewSession = (BrewSessionReference *)*sessionHandle;
		item = NewListItem(brewSession->name, cell);
		HUnlock(sessionHandle);

		AddListItem(dialogState->listHandle, item);
		dialogState->sessionListItems[i] = item;
	}

	SessionListControlUnlock(dialogState);
	SessionListDialogUnlockState(theDialog);
}

short SessionListDialogShow(DialogPtr theDialog)
{
	short itemHit;
	ShowWindow(theDialog);

	do
	{
		ModalDialog(&SessionListEventFilterProc, &itemHit);
	} while (itemHit != ok && itemHit != cancel);

	return SessionListGetSelectedCell(theDialog).v;
}

Cell SessionListGetSelectedCell(DialogPtr theDialog)
{
	const SessionListDialogState *dialogState = SessionListDialogLockState(theDialog);
	Cell cell = {0, 0};
	LGetSelect(TRUE, &cell, dialogState->listHandle);
	SessionListDialogUnlockState(theDialog);
	return cell;
}

void SessionListControlHandleKeyboard(DialogPtr theDialog, char key)
{
	const SessionListDialogState *dialogState = SessionListDialogLockState(theDialog);
	Cell newCell = {0, 0}, oldCell = {0, 0};

	if (dialogState->listHandle == NULL)
	{
		Panic("\pSession list window not set up. Please call SessionListDialogSetUp()");
	}

	if (LGetSelect(TRUE, &oldCell, dialogState->listHandle))
	{
		newCell = DetermineNewCellFromKey(dialogState->listHandle, oldCell, key);
	}
	SelectCell(dialogState->listHandle, oldCell, newCell);
	MakeCellVisible(dialogState->listHandle, newCell);

	SessionListDialogUnlockState(theDialog);
}

static void SessionListControlUpdate(DialogPtr theDialog)
{
	const SessionListDialogState *dialogState = SessionListDialogLockState(theDialog);
	const ListRec *sessionList = SessionListControlLock(dialogState);

	// No need to bracket with Begin/End update because the Dialog event handler does that for us
	// Strange things happen if Being/End update are called here
	SetPort(sessionList->port);
	LUpdate(sessionList->port->visRgn, dialogState->listHandle);
	DrawSessionListBorder(dialogState->listHandle);

	SessionListControlUnlock(dialogState);
	SessionListDialogUnlockState(theDialog);
}

// See: http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/MoreToolbox/MoreToolbox-212.html#HEADING212-0
static Boolean SessionListControlMouseDown(DialogPtr theDialog, EventRecord theEvent)
{
	Boolean itemSelected = false;
	const SessionListDialogState *dialogState = SessionListDialogLockState(theDialog);
	const ListRec *sessionList = SessionListControlLock(dialogState);
	ControlHandle selectedControl;

	SetPort(sessionList->port);
	GlobalToLocal(&theEvent.where);

	if (LClick(theEvent.where, theEvent.modifiers, dialogState->listHandle))
	{
		// Double clicked
		itemSelected = TRUE;
	}

	SessionListControlUnlock(dialogState);
	SessionListDialogUnlockState(theDialog);
	return itemSelected;
}
