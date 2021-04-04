#include "mbConstants.h"
#include "mbWSplash.h"

void SetUpSplashPic(WindowPtr parentWindow);
void DestroySplachPic(WindowPtr parentWindow);

void SetUpSplashPic(WindowPtr parentWindow)
{
	// Variables
	Handle splashWindowStateHandle = (Handle)GetWRefCon(parentWindow);
	SplashWindowState *windowState;

	HLock(splashWindowStateHandle);
	windowState = (SplashWindowState *)*splashWindowStateHandle;

	windowState->picHandle = GetPicture(kSplashImageId);

	SetWindowPic(parentWindow, windowState->picHandle);

	HUnlock(splashWindowStateHandle);
}

void DestroySplachPic(WindowPtr parentWindow)
{
	Handle splashWindowStateHandle = (Handle)GetWRefCon(parentWindow);
	SplashWindowState *windowState;

	HLock(splashWindowStateHandle);
	windowState = (SplashWindowState *)*splashWindowStateHandle;

	if (windowState->picHandle != NULL)
	{
		SetWindowPic(parentWindow, NULL);
		ReleaseResource((Handle)windowState->picHandle);
		windowState->picHandle = NULL;
	}

	HUnlock(splashWindowStateHandle);
}

WindowPtr SetUpSplashWindow(void)
{
	WindowPtr splashWindow = NULL;
	Handle splashWindowStateHandle = NewHandle(sizeof(SplashWindowState));

	splashWindow = GetNewWindow(kSplashWindowId, splashWindow, (WindowPtr)-1L);
	SetWRefCon(splashWindow, (long)splashWindowStateHandle);

	((WindowPeek)splashWindow)->windowKind = kSplashWindowId;

	SetPort(splashWindow);

	SetUpSplashPic(splashWindow);

	return splashWindow;
}

void DestroySplashWindow(WindowPtr window)
{
	DestroySplachPic(window);
	if (window != NULL)
	{
		DisposeWindow(window);
		window = NULL;
	}
}
