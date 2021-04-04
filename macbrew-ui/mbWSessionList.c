#include "mbConstants.h"
#include "mbUtil.h"
#include "mbWSessionList.h"
#include "mbTypes.h"

void DrawSessionListBorder(ListHandle listHandle);
void SetUpSessionList(WindowPtr parentWindow);
void DestroySessionList(WindowPtr parentWindow);
void SetUpButtons(WindowPtr parentWindow);
void DestroyButtons(WindowPtr parentWindow);

void DrawSessionListBorder(ListHandle listHandle) 
{
	Rect border;
	PenState penState;
	ListPtr list = *listHandle;
	
	border = list->rView;
	GetPenState(&penState);
	PenSize(1,1);
	InsetRect(&border, -1, -1);
	FrameRect(&border);
	SetPenState(&penState);
}

void SetUpSessionList(WindowPtr parentWindow)
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
	Handle sessionListWindowStateHandle = (Handle)GetWRefCon(parentWindow);
	SessionListWindowState *windowState;
	Rect dataBounds;
	Point cellSize;
	Rect visibleRect;

	HLock(sessionListWindowStateHandle);
	windowState = (SessionListWindowState *)*sessionListWindowStateHandle;

	visibleRect = parentWindow->portRect;
	visibleRect.top = visibleRect.top + padding;
	visibleRect.left = visibleRect.left + padding;
	visibleRect.right = visibleRect.right - scrollBarWidth - padding;
	visibleRect.bottom = visibleRect.bottom - (padding * 2);

	// start with a list that contains no rows
	SetRect(&dataBounds, 0, 0, columns, 0);

	// let the List Manager calculate the size of a cell}
	SetPt(&cellSize, 0, 0);

	// Create the list
	windowState->listHandle = LNew(&visibleRect, &dataBounds, cellSize, 0, parentWindow, doDraw, noGrow, !includeScrollBar, includeScrollBar);

	DrawSessionListBorder(windowState->listHandle);

	HUnlock(sessionListWindowStateHandle);
}

void DestroySessionList(WindowPtr parentWindow)
{
	Handle sessionListWindowStateHandle = (Handle)GetWRefCon(parentWindow);
	SessionListWindowState *windowState;

	HLock(sessionListWindowStateHandle);
	windowState = (SessionListWindowState *)*sessionListWindowStateHandle;

	if (windowState->listHandle != NULL)
	{
		LDispose(windowState->listHandle);
		windowState->listHandle = NULL;
	}

	HUnlock(sessionListWindowStateHandle);
}

void SetUpButtons(WindowPtr parentWindow)
{

	// Variables
	Handle sessionListWindowStateHandle = (Handle)GetWRefCon(parentWindow);
	SessionListWindowState *windowState;

	HLock(sessionListWindowStateHandle);
	windowState = (SessionListWindowState *)*sessionListWindowStateHandle;

	windowState->cancelButton = GetNewControl(kSessionListCancelButtonId, parentWindow);
	windowState->okButton = GetNewControl(kSessionListOkButtonId, parentWindow);

	HUnlock(sessionListWindowStateHandle);
}

void DestroyButtons(WindowPtr parentWindow)
{
	Handle sessionListWindowStateHandle = (Handle)GetWRefCon(parentWindow);
	SessionListWindowState *windowState;

	HLock(sessionListWindowStateHandle);
	windowState = (SessionListWindowState *)*sessionListWindowStateHandle;

	if (windowState->cancelButton != NULL)
	{
		DisposeControl(windowState->cancelButton);
		windowState->cancelButton = NULL;
	}
	if (windowState->okButton != NULL)
	{
		DisposeControl(windowState->okButton);
		windowState->okButton = NULL;
	}

	HUnlock(sessionListWindowStateHandle);
}

WindowPtr SetUpSessionListWindow(void)
{
	// Parent Window
	Handle sessionListWindowStateHandle;
	WindowPtr sessionListWindow = NULL;
	sessionListWindow = GetNewWindow(kSessionListWindowId, sessionListWindow, (WindowPtr)-1L);

	SetPort(sessionListWindow);
	((WindowPeek)sessionListWindow)->windowKind = kSessionListWindowId;

	sessionListWindowStateHandle = NewHandle(sizeof(SessionListWindowState));
	SetWRefCon(sessionListWindow, (long)sessionListWindowStateHandle);

	// Controls
	SetUpSessionList(sessionListWindow);
	SetUpButtons(sessionListWindow);

	return sessionListWindow;
}

void DestroySessionListWindow(WindowPtr window)
{
	// Controls
	DestroySessionList(window);
	DestroyButtons(window);

	// Parent Window
	if (window != NULL)
	{
		DisposeWindow(window);
	}
}

void UpdateSessionListWindow(WindowPtr window, Sequence *sessionReferences)
{
	short i;
	Cell cell;

	Handle *elements;
	Handle sessionListWindowStateHandle = (Handle)GetWRefCon(window);
	SessionListWindowState *windowState;
	ListRec *sessionList;

	HLock(sessionListWindowStateHandle);
	windowState = (SessionListWindowState *)*sessionListWindowStateHandle;

	if (windowState->listHandle == NULL)
	{
		Panic("\pSession list window not set up. Please call SetUpSessionListWindow()");
	}

	HLock((Handle)windowState->listHandle);
	sessionList = *windowState->listHandle;

	if (sessionList->dataBounds.bottom > 0)
	{
		// TODO: Delete existing rows
		Panic("\pCan only add rows to an empty list at this time");
	}

	for (i = 0; i < sessionReferences->size; i++)
	{
		Handle sessionHandle = sessionReferences->elements[i];
		BrewSessionReference *brewSession = (BrewSessionReference *)*sessionHandle;

		LAddRow(1, i, windowState->listHandle);
		SetPt(&cell, 0, i);

		LSetCell((unsigned char *)brewSession->name + 1, (char)*brewSession->name, cell, windowState->listHandle);
	}

	HUnlock((Handle)windowState->listHandle);
	HUnlock(sessionListWindowStateHandle);
}

// See: http://mirror.informatimago.com/next/developer.apple.com/documentation/mac/MoreToolbox/MoreToolbox-212.html#HEADING212-0
void SessionListMouseDown(WindowPtr window, EventRecord theEvent)
{
	Handle sessionListWindowStateHandle = (Handle)GetWRefCon(window);
	SessionListWindowState *windowState;
	ListRec *sessionList;

	HLock(sessionListWindowStateHandle);
	windowState = (SessionListWindowState *)*sessionListWindowStateHandle;

	if (windowState->listHandle == NULL)
	{
		Panic("\pSession list window not set up. Please call SetUpSessionListWindow()");
	}

	HLock((Handle)windowState->listHandle);
	sessionList = *(windowState->listHandle);

	SetPort(sessionList->port);
	GlobalToLocal(&theEvent.where);
	if (LClick(theEvent.where, theEvent.modifiers, windowState->listHandle))
	{
		// Double click
	}
}

void SessionListUpdate(WindowPtr window)
{
	Handle sessionListWindowStateHandle = (Handle)GetWRefCon(window);
	SessionListWindowState *windowState;
	ListRec *sessionList;

	HLock(sessionListWindowStateHandle);
	windowState = (SessionListWindowState *)*sessionListWindowStateHandle;
	sessionList = *(windowState->listHandle);

	if (windowState->listHandle == NULL)
	{
		Panic("\pSession list window not set up. Please call SetUpSessionListWindow()");
	}

	SetPort(sessionList->port);
	LUpdate(sessionList->port->visRgn, windowState->listHandle);
	DrawSessionListBorder(windowState->listHandle);
	HUnlock(sessionListWindowStateHandle);
}
