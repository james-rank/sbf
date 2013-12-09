#include "sbfKeyValue.h"
#include "sbfKeyValuePrivate.h"

static char*
sbfKeyValueGetValue (sbfKeyValueItem item)
{
    return ((char*)(item + 1)) + item->mKeySize;
}

sbfKeyValue
sbfKeyValue_create (void)
{
    sbfKeyValue table;

    table = xmalloc (sizeof *table);
    RB_INIT (&table->mTree);

    return table;
}

void
sbfKeyValue_destroy (sbfKeyValue table)
{
    sbfKeyValueItem item;
    sbfKeyValueItem item1;

    RB_FOREACH_SAFE (item, sbfKeyValueItemTreeImpl, &table->mTree, item1)
        free (item);
    free (table);
}

sbfKeyValue
sbfKeyValue_copy (sbfKeyValue table)
{
    sbfKeyValue     copy;
    sbfKeyValueItem item;

    copy = sbfKeyValue_create ();
    RB_FOREACH (item, sbfKeyValueItemTreeImpl, &table->mTree)
        sbfKeyValue_put (copy, item->mKey, sbfKeyValueGetValue (item));
    return copy;
}

const char*
sbfKeyValue_get (sbfKeyValue table, const char* key)
{
    struct sbfKeyValueItemImpl impl;
    sbfKeyValueItem            item;

    impl.mKey = key;
    item = RB_FIND (sbfKeyValueItemTreeImpl, &table->mTree, &impl);
    if (item == NULL)
        return NULL;
    return sbfKeyValueGetValue (item);
}

void
sbfKeyValue_put (sbfKeyValue table, const char* key, const char* value)
{
    sbfKeyValueItem item;
    size_t          keySize = strlen (key) + 1;
    size_t          valueSize = strlen (value) + 1;

    sbfKeyValue_remove (table, key);

    item = xmalloc ((sizeof *item) + keySize + valueSize);

    item->mKey = (void*)(item + 1);
    item->mKeySize = keySize;

    memcpy (item + 1, key, keySize);
    memcpy (sbfKeyValueGetValue (item), value, valueSize);

    RB_INSERT (sbfKeyValueItemTreeImpl, &table->mTree, item);
}

void
sbfKeyValue_remove (sbfKeyValue table, const char* key)
{
    struct sbfKeyValueItemImpl impl;
    sbfKeyValueItem            item;

    impl.mKey = key;
    item = RB_FIND (sbfKeyValueItemTreeImpl, &table->mTree, &impl);
    if (item != NULL)
        RB_REMOVE (sbfKeyValueItemTreeImpl, &table->mTree, item);
}
