#include "mbConstants.h"
#include "mbUtil.h"
#include "mbWSessionList.h"

void SetUpSessionList(WindowPtr parentWindow);
void DestroySessionList(void);

WindowPtr sessionListWindow = NULL;
ListHandle sessionList = NULL;

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

	// Variables
	Rect dataBounds;
	Point cellSize;

	Rect visibleRect = sessionListWindow->portRect;
	visibleRect.right = visibleRect.right - scrollBarWidth;

	// start with a list that contains no rows
	SetRect(&dataBounds, 0, 0, columns, 0);

	// let the List Manager calculate the size of a cell}
	SetPt(&cellSize, 0, 0);

	// Create the list
	sessionList = LNew(&visibleRect, &dataBounds, cellSize, 0, sessionListWindow, doDraw, noGrow, !includeScrollBar, includeScrollBar);
}

void DestroySessionList(void)
{
	if (sessionList != NULL)
	{
		LDispose(sessionList);
		sessionList = NULL;
	}
}

void SetUpSessionListWindow(void)
{
	// Parent Window
	sessionListWindow = GetNewWindow(kSessionListWindowId, sessionListWindow, (WindowPtr)-1L);
	SetPort(sessionListWindow);

	// Controls
	SetUpSessionList(sessionListWindow);
}

void DestroySessionListWindow(void)
{
	// Controls
	DestroySessionList();

	// Parent Window
	if (sessionListWindow != NULL)
	{
		DisposeWindow(sessionListWindow);
		sessionListWindow = NULL;
	}
}

void RefreshSessionList(void)
{
	if (sessionListWindow == NULL || sessionList == NULL)
	{
		Panic("\pSession list window not set up. Please call SetUpSessionListWindow()");
	}
}
