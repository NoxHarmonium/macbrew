#include "mbConstants.h"
#include "mbWViewSession.h"

typedef struct ViewSessionWindowState
{
	StringHandle sessionId;
} ViewSessionWindowState;

static void ViewSessionWindowInitState(WindowPtr theWindow);
static ViewSessionWindowState *ViewSessionWindowLockState(WindowPtr theWindow);
static void *ViewSessionWindowUnlockState(WindowPtr theWindow);

static void ViewSessionWindowInitState(WindowPtr theWindow)
{
	const Handle viewSessionWindowStateHandle = NewHandleClear(sizeof(ViewSessionWindowState));
	SetWRefCon(theWindow, (long)viewSessionWindowStateHandle);
	((WindowPeek)theWindow)->windowKind = kViewSessionWindowId;
}

static ViewSessionWindowState *ViewSessionWindowLockState(WindowPtr theWindow)
{
	const Handle viewSessionWindowStateHandle = (Handle)GetWRefCon(theWindow);

	HLock(viewSessionWindowStateHandle);
	return (ViewSessionWindowState *)*viewSessionWindowStateHandle;
}

static void *ViewSessionWindowUnlockState(WindowPtr theWindow)
{
	const Handle viewSessionWindowStateHandle = (Handle)GetWRefCon(theWindow);

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

void SessionViewSetSessionId(WindowPtr window, StringHandle sessionId)
{
	// Temporary function to test session select
	ViewSessionWindowState *windowState = ViewSessionWindowLockState(window);
	windowState->sessionId = sessionId;

	// Set title to demonstrate its working for now
	SetWTitle(window, *sessionId);

	ViewSessionWindowUnlockState(window);
}
