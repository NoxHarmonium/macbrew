struct Sequence;
struct SerialResponse;
struct BrewSession;
struct FermentationData;

void Ping(void);
void FetchBrewSessionReferences(struct Sequence **outSessionReferences);
void FetchBrewSession(StringHandle sessionId, struct BrewSession ***outHandle);
void FetchFermentationData(StringHandle sessionId, struct FermentationData ***outHandle);
void FetchBrewSessionSteps(StringHandle sessionId, struct Sequence **outSessionSteps);
