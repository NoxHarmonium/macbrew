typedef struct BrewSessionReference
{
    Str255 id;
    Str255 batch_code;
    Str255 name;
} BrewSessionReference;

typedef struct Sequence
{
    unsigned short size;
    Handle *elements;
} Sequence;
