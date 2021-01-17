
#include "mbMenus.h"
#include "mbWindow.h"
#include "mbSerial.h"

extern	WindowPtr	mbWindow;
extern	Rect		dragRect;

void InitMacintosh(void);
void HandleMouseDown (EventRecord	*theEvent);
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


void HandleMouseDown (EventRecord	*theEvent)

{
	WindowPtr	theWindow;
	int			windowCode = FindWindow (theEvent->where, &theWindow);
	
    switch (windowCode)
      {
	  case inSysWindow: 
	    SystemClick (theEvent, theWindow);
	    break;
	    
	  case inMenuBar:
	    HandleMenu(MenuSelect(theEvent->where));
	    break;
	    
	  case inDrag:
	  	if (theWindow == mbWindow)
	  	  DragWindow(mbWindow, theEvent->where, &dragRect);
	  	  break;
	  	  
	  case inContent:
	  	if (theWindow == mbWindow)
	  	  {
	  	  if (theWindow != FrontWindow())
	  	    SelectWindow(mbWindow);
	  	  else
	  	    InvalRect(&mbWindow->portRect);
	  	  }
	  	break;
	  	
	  case inGoAway:
	  	if (theWindow == mbWindow && 
	  		TrackGoAway(mbWindow, theEvent->where))
		  HideWindow(mbWindow);
	  	  break;
      }
}


void HandleEvent(void)

{
	int			ok;
	EventRecord	theEvent;

	HiliteMenu(0);
	SystemTask ();		/* Handle desk accessories */
	
	ok = GetNextEvent (everyEvent, &theEvent);
	if (ok)
	  switch (theEvent.what)
	    {
		case mouseDown:
			HandleMouseDown(&theEvent);
			break;
			
		case keyDown: 
		case autoKey:
		    if ((theEvent.modifiers & cmdKey) != 0)
		      {
			  HandleMenu(MenuKey((char) (theEvent.message & charCodeMask)));
			  }
			break;
			
		case updateEvt:
			BeginUpdate(mbWindow);
			//Draw stuff here
			EndUpdate(mbWindow);
		    break;
		    
		case activateEvt:
			InvalRect(&mbWindow->portRect);
			break;
	    }
}



main() {
	Handle* responseData;
	
	InitMacintosh();
	SetUpMenus();
	SetUpWindow();
	
	SetUpSerial();
	SendCommand("1 PING\r");
	ReadResponse(responseData);
	TearDownSerial();
	
	for (;;)
		HandleEvent();
}
/* end main */