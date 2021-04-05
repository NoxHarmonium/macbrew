// Foreard declarations
struct Sequence;

DialogPtr SessionListDialogSetUp(void);
void SessionListDialogDestroy(DialogPtr window);
void SessionListDialogSetSessions(DialogPtr window, struct Sequence *sessionReferences);
void SessionListDialogShow(DialogPtr theDialog);
