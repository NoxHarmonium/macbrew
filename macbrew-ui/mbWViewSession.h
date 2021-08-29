
struct BrewSession;

WindowPtr SessionViewWindowSetUp(void);
void SessionViewWindowDestroy(WindowPtr window);
void SessionViewSetSession(WindowPtr window, struct BrewSession **brewSessionHandle);
