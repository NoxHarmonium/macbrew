
#include "mbConstants.h"
#include "mbMenus.h"
#include "mbWSplash.h"
#include "mbSerial.h"

extern WindowPtr splashWindow;

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
	short windowKind = 0;

	if (theWindow != NULL)
	{
		windowKind = ((WindowPeek)theWindow)->windowKind;
	}

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
		if (windowKind == kSessionListWindowId)
		{
			DragWindow(theWindow, theEvent->where, &(*GetGrayRgn())->rgnBBox);
		}
		break;

	case inContent:
		if (theWindow == splashWindow)
		{
			DestroySplashWindow();
		} else {
			if (theWindow != FrontWindow())
			{
				SelectWindow(theWindow);
			}
			else
			{
				InvalRect(&theWindow->portRect);
			}
			if (windowKind == kSessionListWindowId) {
				SessionListMouseDown(theWindow, *theEvent);
			}
		}
		break;

	case inGoAway:
		if (
			TrackGoAway(theWindow, theEvent->where))
		{
			HideWindow(theWindow);
		}
		break;
	}
}

void HandleEvent(void)
{
	int ok;
	EventRecord theEvent;
	WindowPtr theWindow;
	short windowKind = 0;

	HiliteMenu(0);
	SystemTask(); /* Handle desk accessories */

	ok = GetNextEvent(everyEvent, &theEvent);
	if (ok)
	{
		theWindow = FrontWindow();
		if (theWindow != NULL)
		{
			windowKind = ((WindowPeek)theWindow)->windowKind;
		}

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
			if (theWindow != NULL)
			{
				BeginUpdate(theWindow);
				if (windowKind == kSessionListWindowId)
				{
					SessionListUpdate(theWindow);
				}
				EndUpdate(theWindow);
			}

			break;

		case activateEvt:
			if (splashWindow != NULL)
			{
				InvalRect(&splashWindow->portRect);
			}
			if (windowKind == kSessionListWindowId)
			{
			    // TODO: Where did this go?
				//SessionListActivate((theEvent.modifiers & activeFlag) != 0);
				InvalRect(&theWindow->portRect);
			}
			break;
		}
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
