#include <Quickdraw.h>

#include "mbConstants.h"
#include "mbWViewSession.h"
#include "mbTypes.h"

typedef struct ViewSessionWindowState
{
	BrewSessionHandle brewSessionHandle;
	FermentationDataHandle fermentationDataHandle;
} ViewSessionWindowState;

static void DrawStringHandle(StringHandle stringHandle);
static void ViewSessionWindowInitState(WindowPtr theWindow);
static ViewSessionWindowState *ViewSessionWindowLockState(WindowPtr theWindow);
static void *ViewSessionWindowUnlockState(WindowPtr theWindow);
static void DrawRow(ConstStr255Param title, StringHandle value, short rowNum);
static void SetupQuickDraw(WindowPtr window);

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

static void SetupQuickDraw(WindowPtr window)
{
	// Setup QuickDraw
	SetPort(window);
	TextFont(geneva);
	TextSize(12);
	PenNormal();
}

WindowPtr SessionViewWindowSetUp(void)
{
	WindowPtr viewSessionWindow = NULL;
	viewSessionWindow = GetNewWindow(kViewSessionWindowId, viewSessionWindow, (WindowPtr)-1L);

	ViewSessionWindowInitState(viewSessionWindow);

	SetPort(viewSessionWindow);

	return viewSessionWindow;
}

void SessionViewWindowDestroy(WindowPtr window)
{
	if (window != NULL)
	{
		DisposeWindow(window);
		window = NULL;
	}
}

void SessionViewSetSession(WindowPtr window, BrewSessionHandle brewSessionHandle)
{

	unsigned short rowNum = 1;
	ViewSessionWindowState *windowState = ViewSessionWindowLockState(window);
	BrewSession *brewSession = NULL;
	windowState->brewSessionHandle = brewSessionHandle;

	HLock((Handle)brewSessionHandle);
	brewSession = *brewSessionHandle;

	HLock((Handle)brewSession->recipe_title);
	// Set title to demonstrate its working for now
	SetWTitle(window, *(brewSession->recipe_title));
	HUnlock((Handle)brewSession->recipe_title);

	SetupQuickDraw(window);

	DrawRow("\pCreated At:", brewSession->created_at, rowNum++);
	DrawRow("\pBatch Code:", brewSession->batch_code, rowNum++);
	DrawRow("\pPhase:", brewSession->phase, rowNum++);
	DrawRow("\pStyle:", brewSession->style_name, rowNum++);

	HUnlock((Handle)brewSessionHandle);
	ViewSessionWindowUnlockState(window);
}

void SessionViewSetFermentationData(WindowPtr window, struct FermentationData **fermentationDataHandle)
{
	ViewSessionWindowState *windowState = ViewSessionWindowLockState(window);
	FermentationData *fermentationData = NULL;
	Rect graphFrame;
	unsigned short i, pointSize, maxTemp = 0, maxGravity = 0;

	windowState->fermentationDataHandle = fermentationDataHandle;

	SetupQuickDraw(window);

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

	HUnlock((Handle)fermentationDataHandle);
	ViewSessionWindowUnlockState(window);
}
