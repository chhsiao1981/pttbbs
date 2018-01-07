#include "gtest/gtest.h"
#include "bbs.h"

TEST(pttdb, serialize_uuid_bson) {
    _UUID _uuid;
    UUID uuid;
    char *str;
    char buf[MAX_BUF_SIZE];

    bzero(_uuid, sizeof(_UUID));
    b64_ntop(_uuid, _UUIDLEN, (char *)uuid, UUIDLEN);

    bson_t uuid_bson;
    bson_init(&uuid_bson);
    
    Err error = _serialize_uuid_bson(uuid, MONGO_THE_ID, &uuid_bson);
    str = bson_as_canonical_extended_json (&uuid_bson, NULL);
    strcpy(buf, str);

    printf("str: %s", str);

    bson_free (str);

    bson_destroy(&uuid_bson);

    EXPECT_EQ(error, S_OK);
    EXPECT_STREQ(buf, "{ \"the_id\" : \"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\" }");
}

TEST(pttdb, gen_uuid) {
    UUID uuid;
    time64_t milli_timestamp;

    gen_uuid(uuid);
    uuid_to_milli_timestamp(uuid, &milli_timestamp);

    EXPECT_GTE(milli_timestamp, 1514764800000); // 2018-01-01
    EXPECT_LT(milli_timestamp, 1517443200000);  //2018-02-01
}