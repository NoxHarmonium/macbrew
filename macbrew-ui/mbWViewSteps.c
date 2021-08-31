#include <Quickdraw.h>

#include "mbConstants.h"
#include "mbWViewSteps.h"
#include "mbTypes.h"

typedef struct StepsViewWindowState
{
	Sequence *sessionSteps;
	ControlHandle scrollBar;
	ControlHandle *checkBoxControls;
} StepsViewWindowState;

static void StepsViewWindowInitState(WindowPtr theWindow);
static StepsViewWindowState *StepsViewWindowLockState(WindowPtr theWindow);
static void *StepsViewWindowUnlockState(WindowPtr theWindow);
static void SetupScrollbar(WindowPtr theWindow);
static void AdjustScrollbar(WindowPtr theWindow, StepsViewWindowState *windowState);
static Rect CalculateCheckboxRect(WindowPtr window, short index);
static void OffsetControls(WindowPtr window, short offset);
static void ScrollByOffset(WindowPtr window, ControlHandle targetControl, short previousScrollValue, short newScrollValue);
pascal void HandleScrollButtonClickedProc(ControlHandle controlHandle, short part);

static void StepsViewWindowInitState(WindowPtr theWindow)
{
	Handle viewSessionWindowStateHandle = NewHandleClear(sizeof(StepsViewWindowState));
	SetWRefCon(theWindow, (long)viewSessionWindowStateHandle);
	((WindowPeek)theWindow)->windowKind = kViewStepsWindowId;
}

static StepsViewWindowState *StepsViewWindowLockState(WindowPtr theWindow)
{
	Handle viewSessionWindowStateHandle = (Handle)GetWRefCon(theWindow);

	HLock(viewSessionWindowStateHandle);
	return (StepsViewWindowState *)*viewSessionWindowStateHandle;
}

static void *StepsViewWindowUnlockState(WindowPtr theWindow)
{
	Handle viewSessionWindowStateHandle = (Handle)GetWRefCon(theWindow);

	HUnlock(viewSessionWindowStateHandle);
}

static void AdjustScrollbar(WindowPtr theWindow, StepsViewWindowState *windowState)
{
	ControlHandle scrollBar = windowState->scrollBar;
	Rect portRect = theWindow->portRect;
	short maxHeight, currentValue;

	HideControl(scrollBar);

	MoveControl(scrollBar, portRect.right - kScrollbarAdjust, -1);
	SizeControl(scrollBar, kScrollbarWidth, (portRect.bottom - portRect.top) - (kScrollbarAdjust - kScrollTweek));

	maxHeight = (kBrewSessionWindowVPadding + kBrewSessionStepHeight * windowState->sessionSteps->size);

	SetCtlMax(scrollBar, maxHeight);
	currentValue = GetCtlValue(scrollBar);
	// TODO: Define clamp function?
	if (currentValue < 0)
	{
		SetCtlValue(scrollBar, 0);
	}
	else if (currentValue > maxHeight)
	{
		SetCtlValue(scrollBar, maxHeight);
	}

	ShowControl(scrollBar);
}

static void SetupScrollbar(WindowPtr theWindow)
{
	StepsViewWindowState *windowState = StepsViewWindowLockState(theWindow);

	// TODO: Move to resource?
	windowState->scrollBar = NewControl(theWindow, &theWindow->portRect, "\p", FALSE, 0, 0, 0, scrollBarProc, 0);

	AdjustScrollbar(theWindow, windowState);

	StepsViewWindowUnlockState(theWindow);
}

static Rect CalculateCheckboxRect(WindowPtr window, short index)
{
	Rect r;
	r.left = kBrewSessionWindowHPadding;
	r.top = kBrewSessionWindowVPadding + index * kBrewSessionStepHeight;
	r.right = window->portRect.right - kBrewSessionWindowHPadding;
	r.bottom = kBrewSessionStepHeight + index * kBrewSessionStepHeight;
	return r;
}

static void OffsetControls(WindowPtr window, short offset)
{
	StepsViewWindowState *windowState = StepsViewWindowLockState(window);

	if (windowState->checkBoxControls != NULL)
	{
		short i;
		for (i = 0; i < windowState->sessionSteps->size; i++)
		{
			ControlPtr control;
			Rect rect = CalculateCheckboxRect(window, i);
			ControlHandle controlHandle = windowState->checkBoxControls[i];
			HLock((Handle)controlHandle);
			MoveControl(controlHandle, rect.left, rect.top + offset);
			SizeControl(controlHandle, rect.right - rect.left, rect.bottom - rect.top);
			HUnlock((Handle)controlHandle);
		}
	}

	StepsViewWindowUnlockState(window);
}

static void ScrollByOffset(WindowPtr window, ControlHandle targetControl, short previousScrollValue, short newScrollValue)
{
	short offset = previousScrollValue - newScrollValue;
	Rect newScrollRect = window->portRect;
	RgnHandle newRgn = NewRgn();

	newScrollRect.right = newScrollRect.right - kScrollbarWidth;
	ScrollRect(&newScrollRect, 0, offset, newRgn);
	OffsetControls(window, -newScrollValue);
	InvalRgn(newRgn);

	DisposeRgn(newRgn);
}

pascal void HandleScrollButtonClickedProc(ControlHandle controlHandle, short part)
{
	ControlPtr control;
	StepsViewWindowState *windowState;
	short scrollDistance = 0, minScrollValue = GetCtlMin(controlHandle),
		  maxScrollValue = GetCtlMax(controlHandle), oldScrollValue = GetCtlValue(controlHandle), newScrollValue;
	WindowPtr parentWindow = (*controlHandle)->contrlOwner;

	windowState = StepsViewWindowLockState(parentWindow);
	switch (part)
	{
	case inUpButton:
	case inDownButton:
	{
		scrollDistance = 2;
		break;
	}
	case inPageUp:
	case inPageDown:
	{
		scrollDistance = parentWindow->portRect.bottom - kBrewSessionScrollPageOverlap;
		break;
	}
	}

	if (part == inUpButton || part == inPageUp)
	{
		newScrollValue = oldScrollValue - scrollDistance;
	}
	else
	{
		newScrollValue = oldScrollValue + scrollDistance;
	}

	if (newScrollValue < minScrollValue)
	{
		newScrollValue = minScrollValue;
	}
	if (newScrollValue > maxScrollValue)
	{
		newScrollValue = maxScrollValue;
	}

	ScrollByOffset(parentWindow, controlHandle, oldScrollValue, newScrollValue);
	SetCtlValue(controlHandle, newScrollValue);

	StepsViewWindowUnlockState(parentWindow);
}

WindowPtr StepsViewWindowSetUp(void)
{
	WindowPtr viewSessionWindow = NULL;
	viewSessionWindow = GetNewWindow(kViewStepsWindowId, viewSessionWindow, (WindowPtr)-1L);

	StepsViewWindowInitState(viewSessionWindow);

	SetPort(viewSessionWindow);

	SetWTitle(viewSessionWindow, "\pBrew Session Steps");
	SetupScrollbar(viewSessionWindow);

	return viewSessionWindow;
}

void StepsViewWindowDestroy(WindowPtr window)
{
	if (window != NULL)
	{
		DisposeWindow(window);
		window = NULL;
	}
}

void StepsViewSetSteps(WindowPtr window, Sequence *sessionSteps)
{
	unsigned short i;
	StepsViewWindowState *windowState = StepsViewWindowLockState(window);
	windowState->sessionSteps = sessionSteps;
	windowState->checkBoxControls = (ControlHandle *)NewPtr(sizeof(ControlHandle) * sessionSteps->size);

	for (i = 0; i < sessionSteps->size; i++)
	{
		Rect r = CalculateCheckboxRect(window, i);
		BrewSessionStep *step;
		Handle stepHandle = sessionSteps->elements[i];

		HLock(stepHandle);
		step = (BrewSessionStep *)*stepHandle;
		HLock((Handle)step->description);

		windowState->checkBoxControls[i] = NewControl(window, &r, *(step->description), TRUE, 0, 0, 1, checkBoxProc, i);

		HUnlock((Handle)step->description);
		HUnlock(stepHandle);
	}

	AdjustScrollbar(window, windowState);

	StepsViewWindowUnlockState(window);
}

void StepsViewHandleMouseDown(EventRecord *theEvent, WindowPtr window)
{
	ControlHandle targetControl;
	Point mouse = theEvent->where;
	short part;
	StepsViewWindowState *windowState = StepsViewWindowLockState(window);

	GlobalToLocal(&mouse);
	part = FindControl(mouse, window, &targetControl);
	switch (part)
	{
	// For some reason the Mac developers didn't call this 'inScroll'
	case inThumb:
	{
		RgnHandle newRgn;
		Rect newScrollRect;
		short oldScrollValue = GetCtlValue(targetControl);

		part = TrackControl(targetControl, mouse, NULL);
		// Check that the mouse is still in the control
		if (part == inThumb)
		{
			short newScrollValue = GetCtlValue(targetControl);
			ScrollByOffset(window, targetControl, oldScrollValue, newScrollValue);
		}
		break;
	}
	case inCheckBox:
	{
		part = TrackControl(targetControl, mouse, NULL);
		if (part != 0)
		{
			short currentValue = GetCtlValue(targetControl);
			SetCtlValue(targetControl, 1 - currentValue);
			// TODO: Perist selected steps?
		}
	}
	case inUpButton:
	case inDownButton:
	case inPageUp:
	case inPageDown:
	{
		if (targetControl == windowState->scrollBar)
		{
			part = TrackControl(targetControl, mouse, &HandleScrollButtonClickedProc);
		}
		break;
	}
	}

	StepsViewWindowUnlockState(window);
}

void StepsViewHandleGrow(EventRecord *theEvent, WindowPtr window)
{
	StepsViewWindowState *windowState = StepsViewWindowLockState(window);

	// Redraw the scrollbar in the right place
	AdjustScrollbar(window, windowState);
	// For the controls to resize
	OffsetControls(window, 0);
	// Make the window render everything again
	// TODO: Partial updates
	InvalRect(&window->portRect);

	StepsViewWindowUnlockState(window);
}
