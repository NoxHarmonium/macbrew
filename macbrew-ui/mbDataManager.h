struct Sequence;
struct SerialResponse;
struct BrewSession;

void Ping(void);
void FetchBrewSessionReferences(struct Sequence **outSessionReferences);
void FetchBrewSession(StringHandle sessionId, struct BrewSession **outBrewSession);
