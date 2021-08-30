
struct BrewSession;
struct FermentationData;

WindowPtr SessionViewWindowSetUp(void);
void SessionViewWindowDestroy(WindowPtr window);
void SessionViewSetSession(WindowPtr window, struct BrewSession **brewSessionHandle);
void SessionViewSetFermentationData(WindowPtr window, struct FermentationData **fermentationDataHandle);
