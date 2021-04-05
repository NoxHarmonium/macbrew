#include "mbDialogUtils.h"

pascal void ButtonOutlineDrawProc(DialogPtr theDialog, short theItem);

pascal void ButtonOutlineDrawProc(DialogPtr theDialog, short theItem)
{
	// TODO: Handle colour screens like the example in the Mac Toolbox docs
	short type;
	Rect box;
	Handle itemHdl;

	GetDItem(theDialog, ok, &type, &itemHdl, &box);

	PenSize(3, 3);
	InsetRect(&box, -4, -4);
	FrameRoundRect(&box, 16, 16);
	PenNormal();
}

void SetupDialogButtonOutline(DialogPtr theDialog, short userItemId)
{
	short itemType;
	Handle itemHandle;
	Rect itemRect;

	GetDItem(theDialog, userItemId, &itemType, &itemHandle, &itemRect);
	SetDItem(theDialog, userItemId, itemType,
			 (Handle)&ButtonOutlineDrawProc, &itemRect);
}
