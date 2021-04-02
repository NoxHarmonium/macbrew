// Foreard declarations
struct Sequence;

void SetUpSessionListWindow(void);
void DestroySessionListWindow(void);
void UpdateSessionListWindow(struct Sequence *sessionReferences);
void SessionListMouseDown(EventRecord theEvent);
void SessionListUpdate();
