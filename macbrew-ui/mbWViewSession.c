#include <Quickdraw.h>

#include "mbConstants.h"
#include "mbWViewSession.h"
#include "mbTypes.h"
#include "mbUtil.h"

typedef struct ViewSessionWindowState
{
	BrewSessionHandle brewSessionHandle;
	FermentationDataHandle fermentationDataHandle;
	GrafPtr previousPort;
} ViewSessionWindowState;

static void DrawStringHandle(StringHandle stringHandle);
static void ViewSessionWindowInitState(WindowPtr theWindow);
static ViewSessionWindowState *ViewSessionWindowLockState(WindowPtr theWindow);
static void *ViewSessionWindowUnlockState(WindowPtr theWindow);
static void DrawRow(ConstStr255Param title, StringHandle value, short rowNum);
static void SetupQuickDraw(WindowPtr window, ViewSessionWindowState *windowState);
static void CleanupQuickDraw(ViewSessionWindowState *windowState);
static void DrawSessionInfo(WindowPtr window, ViewSessionWindowState *windowState);
static void DrawGraph(WindowPtr window, ViewSessionWindowState *windowState);

// TODO: Maybe move to a utils file?
static void DrawStringHandle(StringHandle stringHandle)
{
	HLock((Handle)stringHandle);
	DrawString(*stringHandle);
	HUnlock((Handle)stringHandle);
}

static void ViewSessionWindowInitState(WindowPtr theWindow)
{
	Handle viewSessionWindowStateHandle = NewHandleClear(sizeof(ViewSessionWindowState));
	SetWRefCon(theWindow, (long)viewSessionWindowStateHandle);
	((WindowPeek)theWindow)->windowKind = kViewSessionWindowId;
}

static ViewSessionWindowState *ViewSessionWindowLockState(WindowPtr theWindow)
{
	Handle viewSessionWindowStateHandle = (Handle)GetWRefCon(theWindow);

	if (viewSessionWindowStateHandle == NULL)
	{
		Panic("\pCannot lock state before calling ViewSessionWindowInitState!");
	}

	HLock(viewSessionWindowStateHandle);
	return (ViewSessionWindowState *)*viewSessionWindowStateHandle;
}

static void *ViewSessionWindowUnlockState(WindowPtr theWindow)
{
	Handle viewSessionWindowStateHandle = (Handle)GetWRefCon(theWindow);

	HUnlock(viewSessionWindowStateHandle);
}

static void DrawRow(ConstStr255Param title, StringHandle value, short rowNum)
{
	// TODO: Extract label strings to resources
	TextFace(bold);
	MoveTo(10, rowNum * 20);
	DrawString(title);

	TextFace(normal);
	MoveTo(120, rowNum * 20);
	DrawStringHandle(value);
}

static void SetupQuickDraw(WindowPtr window, ViewSessionWindowState *windowState)
{
	if (windowState->previousPort != NULL)
	{
		Panic("\pSetupQuickDraw called twice without a call to CleanupQuickDraw");
	}

	GetPort(&windowState->previousPort);
	// Setup QuickDraw
	SetPort(window);
	TextFont(geneva);
	TextSize(12);
	PenNormal();
}

static void CleanupQuickDraw(ViewSessionWindowState *windowState)
{
	// Restore the port that was set before QuickDraw was set up for this window
	// This function should always be called after SetupQuickDraw or it will panic
	SetPort(windowState->previousPort);
	windowState->previousPort = NULL;
}

static void DrawSessionInfo(WindowPtr window, ViewSessionWindowState *windowState)
{
	unsigned short rowNum = 1;
	BrewSessionHandle brewSessionHandle = windowState->brewSessionHandle;
	BrewSession *brewSession = NULL;

	HLock((Handle)brewSessionHandle);
	brewSession = *brewSessionHandle;

	SetupQuickDraw(window, windowState);

	HLock((Handle)brewSession->recipe_title);
	// Set title to demonstrate its working for now
	SetWTitle(window, *(brewSession->recipe_title));
	HUnlock((Handle)brewSession->recipe_title);

	DrawRow("\pCreated At:", brewSession->created_at, rowNum++);
	DrawRow("\pBatch Code:", brewSession->batch_code, rowNum++);
	DrawRow("\pPhase:", brewSession->phase, rowNum++);
	DrawRow("\pStyle:", brewSession->style_name, rowNum++);

	CleanupQuickDraw(windowState);

	HUnlock((Handle)brewSessionHandle);
}

static void DrawGraph(WindowPtr window, ViewSessionWindowState *windowState)
{
	FermentationDataHandle fermentationDataHandle = windowState->fermentationDataHandle;
	FermentationData *fermentationData = NULL;
	Rect graphFrame;
	unsigned short i, pointSize, maxTemp = 0, maxGravity = 0;

	SetupQuickDraw(window, windowState);

	HLock((Handle)fermentationDataHandle);
	fermentationData = *fermentationDataHandle;

	pointSize = kGraphWidth / fermentationData->graph->size;

	graphFrame.top = 100;
	graphFrame.left = 20;
	graphFrame.right = graphFrame.left + kGraphWidth;
	graphFrame.bottom = graphFrame.top + kGraphHeight;

	FrameRect(&graphFrame);

	for (i = 0; i < fermentationData->graph->size; i++)
	{
		// TODO: These points probably shouldn't be handles
		// Should be packed into an array probably
		Handle dataPointHandle = fermentationData->graph->elements[i];
		FermentationPoint *dataPoint;

		HLock(dataPointHandle);
		dataPoint = (FermentationPoint *)*dataPointHandle;

		if (dataPoint->temp > maxTemp)
		{
			maxTemp = dataPoint->temp;
		}

		if (dataPoint->gravity > maxGravity)
		{
			maxGravity = dataPoint->gravity;
		}

		HUnlock(dataPointHandle);
	}

	if (maxTemp == 0 || maxGravity == 0)
	{
		HUnlock((Handle)fermentationDataHandle);
		// Protect against divide by zero crashes
		return;
	}

	// TODO: Labels for the graph
	// TODO: Legend for the graph
	// TODO: Extract things into functions for DRY
	for (i = 0; i < fermentationData->graph->size - 1; i++)
	{
		// TODO: These points probably shouldn't be handles
		// Should be packed into an array probably
		Handle dataPointHandle = fermentationData->graph->elements[i];
		Handle nextDataPointHandle = fermentationData->graph->elements[i + 1];
		FermentationPoint *dataPoint, *nextDataPoint;
		unsigned short bottom = graphFrame.top + kGraphHeight;
		unsigned short tempScaleFactor = (kGraphHeight - 20) / maxTemp;
		unsigned short gravityScaleFactor = (kGraphHeight - 20) / maxGravity;

		HLock(dataPointHandle);
		HLock(nextDataPointHandle);
		dataPoint = (FermentationPoint *)*dataPointHandle;
		nextDataPoint = (FermentationPoint *)*nextDataPointHandle;

		PenNormal();
		MoveTo(graphFrame.left + (pointSize * i), bottom - (dataPoint->temp * tempScaleFactor));
		LineTo(graphFrame.left + (pointSize * (i + 1)), bottom - (nextDataPoint->temp * tempScaleFactor));

		PenPat(gray);
		MoveTo(graphFrame.left + (pointSize * i), bottom - (dataPoint->gravity * gravityScaleFactor));
		LineTo(graphFrame.left + (pointSize * (i + 1)), bottom - (nextDataPoint->gravity * gravityScaleFactor));

		HUnlock(nextDataPointHandle);
		HUnlock(dataPointHandle);
	}

	CleanupQuickDraw(windowState);

	HUnlock((Handle)fermentationDataHandle);
}

WindowPtr SessionViewWindowSetUp(void)
{
	WindowPtr viewSessionWindow = NULL;
	viewSessionWindow = GetNewWindow(kViewSessionWindowId, viewSessionWindow, (WindowPtr)-1L);

	ViewSessionWindowInitState(viewSessionWindow);

	// SetPort(viewSessionWindow);

	return viewSessionWindow;
}

void SessionViewWindowDestroy(WindowPtr window)
{
	if (window != NULL)
	{
		// Clean up the state record
		Handle viewSessionWindowStateHandle = (Handle)GetWRefCon(window);
		if (viewSessionWindowStateHandle != NULL)
		{
			DisposeHandle(viewSessionWindowStateHandle);
			SetWRefCon(window, (long)NULL);
		}

		// DisposeWindow automatically cleans up all the controls by calling DisposeControl for us
		DisposeWindow(window);
		window = NULL;
	}
}

void SessionViewSetSession(WindowPtr window, BrewSessionHandle brewSessionHandle)
{
	ViewSessionWindowState *windowState = ViewSessionWindowLockState(window);
	windowState->brewSessionHandle = brewSessionHandle;

	DrawSessionInfo(window, windowState);

	ViewSessionWindowUnlockState(window);
}

void SessionViewSetFermentationData(WindowPtr window, struct FermentationData **fermentationDataHandle)
{
	ViewSessionWindowState *windowState = ViewSessionWindowLockState(window);
	windowState->fermentationDataHandle = fermentationDataHandle;

	DrawGraph(window, windowState);

	ViewSessionWindowUnlockState(window);
}

void SessionViewUpdate(WindowPtr window)
{
	ViewSessionWindowState *windowState = ViewSessionWindowLockState(window);
	BrewSessionHandle brewSessionHandle = windowState->brewSessionHandle;
	FermentationDataHandle fermentationDataHandle = windowState->fermentationDataHandle;

	if (brewSessionHandle != NULL)
	{
		DrawSessionInfo(window, windowState);
	}

	if (fermentationDataHandle != NULL)
	{
		DrawGraph(window, windowState);
	}

	ViewSessionWindowUnlockState(window);
}
