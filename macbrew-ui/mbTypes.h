typedef struct Sequence
{
    unsigned short size;
    Handle *elements;
} Sequence;

typedef struct BrewSessionReference
{
    StringHandle id;
    StringHandle batch_code;
    StringHandle name;
} BrewSessionReference;

typedef struct BrewSession
{
    StringHandle id;
    StringHandle phase;
    StringHandle batch_code;
    StringHandle recipe_title;
    StringHandle recipe_id;
} BrewSession;

typedef struct Fermentable
{
    StringHandle name;
    StringHandle amount;
} Fermentable;

typedef struct Hop
{
    StringHandle name;
    StringHandle amount;
    StringHandle time;
} Hop;

typedef struct Yeast
{
    StringHandle name;
    StringHandle amount;
} Yeast;

typedef struct Recipe
{
    StringHandle name;
    StringHandle version;
    StringHandle recipe_type;
    Sequence *fermentables;
    Sequence *hops;
    Sequence *yeast;
} Recipe;

typedef BrewSessionReference **BrewSessionReferenceHandle;
typedef BrewSession **BrewSessionHandle;
