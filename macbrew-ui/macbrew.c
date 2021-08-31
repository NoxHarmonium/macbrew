
#include "mbConstants.h"
#include "mbMenus.h"
#include "mbWSplash.h"
#include "mbDSessionList.h"
#include "mbSerial.h"

#include "mbWViewSession.h"
#include "mbWViewSteps.h"

static void InitMacintosh(void);
static void HandleZoomWindow(WindowPtr thisWindow, EventRecord *event, short zoomInOrOut, short windowKind);
static void HandleGrowWindow(WindowPtr thisWindow, EventRecord *event, short windowKind);
static void HandleMouseDown(EventRecord *theEvent);
static void HandleEvent(void);

static void InitMacintosh(void)
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

static void HandleZoomWindow(WindowPtr thisWindow, EventRecord *event, short zoomInOrOut, short windowKind)
{
	// TODO: Add all that boring logic to handle multiple screens
	ZoomWindow(thisWindow, zoomInOrOut, thisWindow == FrontWindow());

	// 	Invalidate window
	if (windowKind == kViewStepsWindowId)
	{
		StepsViewHandleGrow(event, thisWindow);
	}
}

static void HandleGrowWindow(WindowPtr thisWindow, EventRecord *event, short windowKind)
{
	Rect oldViewRect = thisWindow->portRect, limitRect;
	long growSize;

	SetRect(&limitRect, kMinWindowSize, kMinWindowSize, kMaxWindowSize, kMaxWindowSize);

	// Track the grow button action
	growSize = GrowWindow(thisWindow, event->where, &limitRect);

	if (growSize != 0)
	{
		Rect newViewRect;
		// Do the actual resize
		SizeWindow(thisWindow, LoWord(growSize), HiWord(growSize), TRUE);

		// Invalidate window
		if (windowKind == kViewStepsWindowId)
		{
			StepsViewHandleGrow(event, thisWindow);
		}

		// TODO: Work out how to validate the part of the window that hasn't changed (see docs)
		// without breaking the grow button rendering
	}
}

static void HandleMouseDown(EventRecord *theEvent)
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
	{
		DragWindow(theWindow, theEvent->where, &(*GetGrayRgn())->rgnBBox);
		break;
	}

	case inContent:
		if (theWindow != FrontWindow())
		{
			SelectWindow(theWindow);
		}

		if (windowKind == kSplashWindowId)
		{
			// Close the splash screen if clicked
			DestroySplashWindow(theWindow);
		}
		else if (windowKind == kViewStepsWindowId)
		{
			StepsViewHandleMouseDown(theEvent, theWindow);
		}
		break;

	case inGoAway:
		if (
			TrackGoAway(theWindow, theEvent->where))
		{
			HideWindow(theWindow);
		}
		break;

	case inGrow:
		// Can only resize the steps window at this point
		HandleGrowWindow(theWindow, theEvent, windowKind);
		break;

	case inZoomIn:
	case inZoomOut:
		if (TrackBox(theWindow, theEvent->where, windowCode))
		{
			HandleZoomWindow(theWindow, theEvent, windowCode, windowKind);
		}
		break;
	}
}

static void HandleEvent(void)
{
	int ok;
	EventRecord theEvent;
	WindowPtr theWindow;

	HiliteMenu(0);
	SystemTask(); /* Handle desk accessories */

	// TODO: Replace SystemTask/GetNextEvent with WaitNextEvent (System 7+ only?)
	ok = GetNextEvent(everyEvent, &theEvent);
	if (ok)
	{
		short windowKind;
		theWindow = FrontWindow();
		windowKind = ((WindowPeek)theWindow)->windowKind;

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
				RgnHandle oldClipRegion = theWindow->clipRgn;
				RgnHandle clipRegion = NewRgn();

				BeginUpdate(theWindow);

				// TODO: I don't think we need to erase/redraw on every update cycle
				// but it is the only way I can get it to work for now
				EraseRect(&theWindow->portRect);
				UpdateControls(theWindow, theWindow->visRgn);

				// Clip out the frame for the bottom scrollbar which we don't use right now
				SetRectRgn(clipRegion, theWindow->portRect.right - kScrollbarAdjust, theWindow->portRect.top, theWindow->portRect.right, theWindow->portRect.bottom);
				theWindow->clipRgn = clipRegion;
				DrawGrowIcon(theWindow);
				theWindow->clipRgn = oldClipRegion;
				DisposeRgn(clipRegion);

				// Window specific update logic
				if (windowKind == kViewSessionWindowId)
				{
					SessionViewUpdate(theWindow);
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
