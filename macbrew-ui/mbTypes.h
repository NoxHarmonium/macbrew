typedef struct BrewSessionReference
{
    StringHandle id;
    StringHandle batch_code;
    StringHandle name;
} BrewSessionReference;

typedef BrewSessionReference **BrewSessionReferenceHandle;

typedef struct Sequence
{
    unsigned short size;
    Handle *elements;
} Sequence;
