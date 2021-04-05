#include "mbConstants.h"
#include "mbListUtils.h"

void SelectCell(ListHandle listHandle, Cell oldCell, Cell newCell)
{
	LSetSelect(FALSE, oldCell, listHandle);
	LSetSelect(TRUE, newCell, listHandle);
}

void MakeCellVisible(ListHandle listHandle, Cell newCell)
{
	ListRec *sessionList = *listHandle;
	Rect visibleRect = sessionList->visible;
	if (!PtInRect(newCell, &visibleRect))
	{
		short newCol, newRow = 0;

		if (newCell.h > visibleRect.right - 1)
		{
			newCol = newCell.h - visibleRect.right + 1;
		}
		else if (newCell.h < visibleRect.left)
		{
			newCol = newCell.h - visibleRect.left;
		}
		if (newCell.v > visibleRect.bottom - 1)
		{
			newRow = newCell.v - visibleRect.bottom + 1;
		}
		else if (newCell.v < visibleRect.top)
		{
			newRow = newCell.v - visibleRect.top;
		}
		LScroll(newCol, newRow, listHandle);
	}
}

Cell DetermineNewCellFromKey(ListHandle listHandle, Cell oldCellLoc, char keyHit)
{
	ListPtr list = *listHandle;
	short rowCount = list->dataBounds.bottom - list->dataBounds.top;
	Cell newCell = oldCellLoc;

	if (keyHit == kUpArrow && oldCellLoc.v > 0)
	{
		newCell.v = oldCellLoc.v - 1;
	}
	else if (keyHit == kDownArrow && oldCellLoc.v < rowCount - 1)
	{
		newCell.v = oldCellLoc.v + 1;
	}

	return newCell;
}

void DrawSessionListBorder(ListHandle listHandle)
{
	Rect border;
	PenState penState;
	ListPtr list = *listHandle;

	border = list->rView;
	GetPenState(&penState);
	PenSize(1, 1);
	InsetRect(&border, -1, -1);
	FrameRect(&border);
	SetPenState(&penState);
}
