
struct BrewSessionReference;

WindowPtr SessionViewWindowSetUp(void);
void SessionViewWindowDestroy(WindowPtr window);
void SessionViewSetSession(WindowPtr window, struct BrewSessionReference **brewSessionReferenceHandle);
