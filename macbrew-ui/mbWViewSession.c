#include "mbConstants.h"
#include "mbWViewSession.h"
#include "mbTypes.h"

typedef struct ViewSessionWindowState
{
	BrewSessionReferenceHandle brewSessionReferenceHandle;
} ViewSessionWindowState;

static void ViewSessionWindowInitState(WindowPtr theWindow);
static ViewSessionWindowState *ViewSessionWindowLockState(WindowPtr theWindow);
static void *ViewSessionWindowUnlockState(WindowPtr theWindow);

static void ViewSessionWindowInitState(WindowPtr theWindow)
{
	Handle viewSessionWindowStateHandle = NewHandleClear(sizeof(ViewSessionWindowState));
	SetWRefCon(theWindow, (long)viewSessionWindowStateHandle);
	((WindowPeek)theWindow)->windowKind = kViewSessionWindowId;
}

static ViewSessionWindowState *ViewSessionWindowLockState(WindowPtr theWindow)
{
	Handle viewSessionWindowStateHandle = (Handle)GetWRefCon(theWindow);

	HLock(viewSessionWindowStateHandle);
	return (ViewSessionWindowState *)*viewSessionWindowStateHandle;
}

static void *ViewSessionWindowUnlockState(WindowPtr theWindow)
{
	Handle viewSessionWindowStateHandle = (Handle)GetWRefCon(theWindow);

	HUnlock(viewSessionWindowStateHandle);
}

WindowPtr SessionViewWindowSetUp(void)
{
	WindowPtr viewSessionWindow = NULL;
	viewSessionWindow = GetNewWindow(kViewSessionWindowId, viewSessionWindow, (WindowPtr)-1L);

	ViewSessionWindowInitState(viewSessionWindow);

	SetPort(viewSessionWindow);

	return viewSessionWindow;
}

void SessionViewWindowDestroy(WindowPtr window)
{
	if (window != NULL)
	{
		DisposeWindow(window);
		window = NULL;
	}
}

void SessionViewSetSession(WindowPtr window, BrewSessionReferenceHandle brewSessionReferenceHandle)
{
	// Temporary function to test session select
	ViewSessionWindowState *windowState = ViewSessionWindowLockState(window);
	BrewSessionReference *brewSessionReference = NULL;
	windowState->brewSessionReferenceHandle = brewSessionReferenceHandle;
	HLock((Handle)brewSessionReferenceHandle);
	brewSessionReference = *brewSessionReferenceHandle;
	HLock((Handle)brewSessionReference->name);

	// Set title to demonstrate its working for now
	SetWTitle(window, *(brewSessionReference->name));

	HUnlock((Handle)brewSessionReference->name);
	HUnlock((Handle)brewSessionReferenceHandle);
	ViewSessionWindowUnlockState(window);
}
