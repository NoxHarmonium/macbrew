
struct BrewSession;
struct FermentationData;

WindowPtr StepsViewWindowSetUp(void);
void StepsViewWindowDestroy(WindowPtr window);
void StepsViewSetSteps(WindowPtr window, struct Sequence *sessionSteps);
void StepsViewHandleMouseDown(EventRecord *theEvent, WindowPtr window);
void StepsViewHandleGrow(EventRecord *theEvent, WindowPtr window);
