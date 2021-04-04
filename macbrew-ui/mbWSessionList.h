// Foreard declarations
struct Sequence;

typedef struct SessionListWindowState
{
    ListHandle listHandle;
    ControlHandle cancelButton;
    ControlHandle okButton;
} SessionListWindowState;

WindowPtr SetUpSessionListWindow(void);
void DestroySessionListWindow(WindowPtr window);
void UpdateSessionListWindow(WindowPtr window, struct Sequence *sessionReferences);
void SessionListMouseDown(WindowPtr window, EventRecord theEvent);
void SessionListUpdate(WindowPtr window);
