
#include "mbConstants.h"
#include "mbMenus.h"
#include "mbWSplash.h"
#include "mbSerial.h"

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
		if (windowKind == kSessionListWindowId)
		{
			DragWindow(theWindow, theEvent->where, &(*GetGrayRgn())->rgnBBox);
		}
		break;

	case inContent:
		if (theWindow != FrontWindow())
		{
			SelectWindow(theWindow);
		}
		else
		{
			InvalRect(&theWindow->portRect);
		}

		if (windowKind == kSplashWindowId)
		{
			// Close the splash screen if clicked
			DestroySplashWindow(theWindow);
		}
		else if (windowKind == kSessionListWindowId)
		{
			SessionListMouseDown(theWindow, *theEvent);
		}
		break;

	case inGoAway:
		if (
			TrackGoAway(theWindow, theEvent->where))
		{
			HideWindow(theWindow);
			if (windowKind == kSessionListWindowId)
			{
				DestroySessionListWindow(theWindow);
			}
		}
		break;
	}
}

void HandleEvent(void)
{
	int ok;
	EventRecord theEvent;
	WindowPtr theWindow;

	HiliteMenu(0);
	SystemTask(); /* Handle desk accessories */

	ok = GetNextEvent(everyEvent, &theEvent);
	if (ok)
	{
		short windowKind = 0;

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
			if (theWindow != NULL)
			{
				InvalRect(&theWindow->portRect);
			}
			break;
		}
	}
}

void main()
{

	InitMacintosh();
	SetUpMenus();
	SetUpSplashWindow();

	for (;;)
		HandleEvent();
}
