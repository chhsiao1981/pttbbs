#include "gtest/gtest.h"
#include "bbs.h"

TEST(pttdb, serialize_uuid_bson) {
    UUID uuid;
    bzero(uuid, sizeof(UUID));

    bson_t uuid_bson;
    bson_init(&uuid_bson);
    
    Err error = _serialize_uuid_bson(uuid, &uuid_bson);

    bson_destroy(&uuid_bson);

    EXPECT_EQ(error, S_OK);
}
