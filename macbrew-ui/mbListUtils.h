

typedef struct ListItem
{
    StringPtr string;
    unsigned char *stringData;
    unsigned char stringLength;
    Cell cell;
} ListItem;

void SelectCell(ListHandle listHandle, Cell oldCell, Cell newCell);
void MakeCellVisible(ListHandle listHandle, Cell newCell);
Cell DetermineNewCellFromKey(ListHandle listHandle, Cell oldCellLoc, char keyHit);
void DrawSessionListBorder(ListHandle listHandle);
ListItem *NewListItem(StringHandle label, Cell cell);
void DestroyListItem(ListItem *listItem);
void AddListItem(ListHandle listHandle, ListItem *listItemHandle);
