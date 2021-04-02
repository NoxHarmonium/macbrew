
#include "mbMenus.h"
#include "mbWSplash.h"
#include "mbSerial.h"

extern WindowPtr splashWindow;
// extern Rect dragRect;
extern WindowPtr sessionListWindow;

void InitMacintosh(void);
void HandleMouseDown(EventRecord *theEvent);
void HandleEvent(void);

void InitMacintosh(void)
{
	MaxApplZone();

	InitGraf(&thePort);
	InitFonts();
	FlushEvents(everyEvent, 0);
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(0L);
	InitCursor();
}

void HandleMouseDown(EventRecord *theEvent)
{
	WindowPtr theWindow;
	int windowCode = FindWindow(theEvent->where, &theWindow);

	switch (windowCode)
	{
	case inSysWindow:
		SystemClick(theEvent, theWindow);
		break;

	case inMenuBar:
		HandleMenu(MenuSelect(theEvent->where));
		break;

	case inDrag:
		// TODO: Why isn't drag working?
		if (theWindow == sessionListWindow)
		{
			DragWindow(sessionListWindow, theEvent->where, &(*GetGrayRgn())->rgnBBox);
		}
		break;

	case inContent:
		if (theWindow == splashWindow)
		{
			DestroySplashWindow();
		}
		if (theWindow == sessionListWindow)
		{
			if (sessionListWindow != FrontWindow())
			{
				SelectWindow(sessionListWindow);
			}
			else
			{
				InvalRect(&sessionListWindow->portRect);
			}
			SessionListMouseDown(*theEvent);
		}
		break;

	case inGoAway:
		if (theWindow == splashWindow &&
			TrackGoAway(splashWindow, theEvent->where))
			HideWindow(splashWindow);
		break;
	}
}

void HandleEvent(void)
{
	int ok;
	EventRecord theEvent;

	HiliteMenu(0);
	SystemTask(); /* Handle desk accessories */

	ok = GetNextEvent(everyEvent, &theEvent);
	if (ok)
		switch (theEvent.what)
		{
		case mouseDown:
			HandleMouseDown(&theEvent);
			break;

		case keyDown:
		case autoKey:
			if ((theEvent.modifiers & cmdKey) != 0)
			{
				HandleMenu(MenuKey((char)(theEvent.message & charCodeMask)));
			}
			break;

		case updateEvt:
			if (sessionListWindow != NULL)
			{
				BeginUpdate(sessionListWindow);
				SessionListUpdate();
				EndUpdate(sessionListWindow);
			}

			break;

		case activateEvt:
			if (splashWindow != NULL)
			{
				InvalRect(&splashWindow->portRect);
			}
			if (sessionListWindow != NULL)
			{
				SessionListActivate((theEvent.modifiers & activeFlag) != 0);
				InvalRect(&sessionListWindow->portRect);
			}
			break;
		}
}

main()
{
	SerialResponse *responseData;
	char message[255];

	InitMacintosh();
	SetUpMenus();
	SetUpSplashWindow();

	for (;;)
		HandleEvent();
}
