

#include "mbWindow.h"

WindowPtr mbWindow;
Rect dragRect;
Rect windowBounds = {40, 40, 150, 150};
Rect circleStart = {10, 10, 100, 100};
int width = 5;

void SetUpWindow(void)
{
	dragRect = screenBits.bounds;

	mbWindow = NewWindow(0L, &windowBounds, "\pMacBrew", true, noGrowDocProc, (WindowPtr)-1L, true, 0);
	SetPort(mbWindow);
}

void DrawBullseye(short active)
{
	Rect myRect;
	int color = 1;

	SetPort(mbWindow);
	EraseRect(&circleStart);
	myRect = circleStart;

	while (myRect.left < myRect.right)
	{
		FillOval(&myRect, color ? (active ? black : gray) : white);
		InsetRect(&myRect, width, width);
		color = !color;
	}
}
