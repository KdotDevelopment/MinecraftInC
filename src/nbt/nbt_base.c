#include <nbt/nbt_base.h>

#include <string.h>

nbt_base_t *nbt_set_key(nbt_base_t *base, char *key) {
    strncpy(base->key, key, MAX_NBT_KEY_LENGTH);
    base->key[MAX_NBT_KEY_LENGTH - 1] = '\0';
    return base;
}