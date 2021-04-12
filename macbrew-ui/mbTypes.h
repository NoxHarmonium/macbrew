typedef struct BrewSessionReference
{
    StringHandle id;
    StringHandle batch_code;
    StringHandle name;
} BrewSessionReference;

typedef struct Sequence
{
    unsigned short size;
    Handle *elements;
} Sequence;
