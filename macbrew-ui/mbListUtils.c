
#include <string.h>

#include "mbConstants.h"
#include "mbListUtils.h"

static StringPtr CopyStringHandleToPtr(StringHandle sourceString);

void SelectCell(ListHandle listHandle, Cell oldCell, Cell newCell)
{
	LSetSelect(FALSE, oldCell, listHandle);
	LSetSelect(TRUE, newCell, listHandle);
}

void MakeCellVisible(ListHandle listHandle, Cell newCell)
{
	ListRec *list = *listHandle;
	Rect visibleRect = list->visible;
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

static StringPtr CopyStringHandleToPtr(StringHandle sourceString)
{
	StringPtr src, dest;
	unsigned char length;

	HLock((Handle)sourceString);
	src = *sourceString;
	length = (unsigned char)*src;
	dest = (StringPtr)NewPtr(length + 1);
	memcpy(dest, src, length + 1);
	HUnlock((Handle)sourceString);

	return dest;
}

ListItem *NewListItem(StringHandle label, Cell cell)
{
	StringPtr copiedString;
	ListItem *listItem = (ListItem *)NewPtr(sizeof(ListItem));

	listItem->string = NULL;
	listItem->cell = cell;
	copiedString = CopyStringHandleToPtr(label);
	listItem->string = copiedString;
	listItem->stringData = (unsigned char *)copiedString + 1;
	listItem->stringLength = (char)*copiedString;

	return listItem;
}

void DestroyListItem(ListItem *listItem)
{
	if (listItem->string != NULL)
	{
		DisposePtr((Ptr)listItem->string);
		listItem->string = NULL;
		listItem->stringData = NULL;
		listItem->stringLength = 0;
	}
	DisposePtr((Ptr)listItem);
}

void AddListItem(ListHandle listHandle, ListItem *listItem)
{
	LAddRow(1, listItem->cell.v, listHandle);

	LSetCell(listItem->stringData, listItem->stringLength, listItem->cell, listHandle);
	if (listItem->cell.v == 0)
	{
		LSetSelect(TRUE, listItem->cell, listHandle);
	}
}
