#include "mbConstants.h"
#include "mbWViewSession.h"
#include "mbTypes.h"

typedef struct ViewSessionWindowState
{
	BrewSessionHandle brewSessionHandle;
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

void SessionViewSetSession(WindowPtr window, BrewSessionHandle brewSessionHandle)
{
	// Temporary function to test session select
	ViewSessionWindowState *windowState = ViewSessionWindowLockState(window);
	BrewSession *brewSession = NULL;
	windowState->brewSessionHandle = brewSessionHandle;
	HLock((Handle)brewSessionHandle);
	brewSession = *brewSessionHandle;
	HLock((Handle)brewSession->recipe_title);
	HLock((Handle)brewSession->phase);

	// Set title to demonstrate its working for now
	SetWTitle(window, *(brewSession->recipe_title));

	SetPort(window);
	TextFont(geneva);
	TextSize(12);

	MoveTo(10, 10);
	DrawString(*(brewSession->phase));

	HUnlock((Handle)brewSession->phase);
	HUnlock((Handle)brewSession->recipe_title);
	HUnlock((Handle)brewSessionHandle);
	ViewSessionWindowUnlockState(window);
}
