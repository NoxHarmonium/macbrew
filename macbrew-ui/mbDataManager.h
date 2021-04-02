typedef struct BrewSessionReference
{
    Str255 id;
    Str255 batch_code;
    Str255 name;
} BrewSessionReference;

void Ping();
void FetchBrewSessionReferences(Handle *sessionReferences);
