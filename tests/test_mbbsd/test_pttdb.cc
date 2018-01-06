#include "gtest/gtest.h"
#include "bbs.h"

TEST(pttdb, serialize_uuid_bson) {
    UUID uuid;
    char *str;
    char buf[MAX_BUF_SIZE];


    bzero(uuid, sizeof(UUID));    

    bson_t uuid_bson;
    bson_init(&uuid_bson);
    
    Err error = _serialize_uuid_bson(uuid, &uuid_bson);
    str = bson_as_canonical_extended_json (&uuid_bson, NULL);
    strcpy(buf, str);

    printf("str: %s", str);

    bson_free (str);

    bson_destroy(&uuid_bson);

    EXPECT_EQ(error, S_OK);
}
