#include "gtest/gtest.h"
#include "pttdb.h"

TEST(pttdb, serialize_uuid_bson) {
    UUID uuid;
    bzero(uuid, sizeof(UUID));

    bson_t uuid_bson;
    bson_init(&uuid);
    
    Err error = _serialize_uuid_bson(uuid, &uuid_bson);

    bson_destroy(&uuid_bson);

    EXPECT_EQ(err, S_OK);
}
