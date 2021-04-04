
typedef struct SplashWindowState
{
	PicHandle picHandle;
} SplashWindowState;

WindowPtr SetUpSplashWindow(void);
void DestroySplashWindow(WindowPtr window);
