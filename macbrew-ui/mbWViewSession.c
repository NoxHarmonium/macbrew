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
	HLock((Handle)brewSession->created_at);
	HLock((Handle)brewSession->batch_code);
	HLock((Handle)brewSession->phase);
	HLock((Handle)brewSession->style_name);

	// Set title to demonstrate its working for now
	SetWTitle(window, *(brewSession->recipe_title));

	// Setup QuickDraw
	SetPort(window);
	TextFont(geneva);
	TextSize(12);

	// TODO: Extract label strings to resources
	TextFace(bold);
	MoveTo(10, 20);
	DrawString("\pCreated at:");

	TextFace(normal);
	MoveTo(120, 20);
	DrawString(*(brewSession->created_at));

	TextFace(bold);
	MoveTo(10, 40);
	DrawString("\pBatch Code:");

	TextFace(normal);
	MoveTo(120, 40);
	DrawString(*(brewSession->batch_code));

	TextFace(bold);
	MoveTo(10, 60);
	DrawString("\pPhase:");

	TextFace(normal);
	MoveTo(120, 60);
	DrawString(*(brewSession->phase));

	TextFace(bold);
	MoveTo(10, 80);
	DrawString("\pStyle:");

	TextFace(normal);
	MoveTo(120, 80);
	DrawString(*(brewSession->style_name));

	HUnlock((Handle)brewSession->style_name);
	HUnlock((Handle)brewSession->phase);
	HUnlock((Handle)brewSession->batch_code);
	HUnlock((Handle)brewSession->created_at);
	HUnlock((Handle)brewSession->recipe_title);
	HUnlock((Handle)brewSessionHandle);
	ViewSessionWindowUnlockState(window);
}
