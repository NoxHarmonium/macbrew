#include "mbConstants.h"
#include "mbWSplash.h"

WindowPtr splashWindow = NULL;
PicHandle splashPicture = NULL;

void SetUpSplashWindow(void)
{
	splashWindow = GetNewWindow(kSplashWindowId, splashWindow, (WindowPtr)-1L);
	splashPicture = GetPicture(kSplashImageId);
	SetWindowPic(splashWindow, splashPicture);
	SetPort(splashWindow);
}

void DestroySplashWindow(void)
{
	if (splashPicture != NULL)
	{
		if (splashWindow != NULL)
		{
			SetWindowPic(splashWindow, NULL);
		}
		ReleaseResource((Handle)splashPicture);
		splashPicture = NULL;
	}
	if (splashWindow != NULL)
	{
		DisposeWindow(splashWindow);
		splashWindow = NULL;
	}
}
