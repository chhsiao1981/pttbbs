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

    bson_free (str);

    bson_destroy(&uuid_bson);

    EXPECT_EQ(S_OK, error);
    EXPECT_STREQ("{ \"the_id\" : \"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\" }", buf);
}

TEST(pttdb, serialize_content_uuid_bson) {
    _UUID _uuid;
    UUID uuid;
    char *str;
    char buf[MAX_BUF_SIZE];

    bzero(_uuid, sizeof(_UUID));
    b64_ntop(_uuid, _UUIDLEN, (char *)uuid, UUIDLEN);

    bson_t uuid_bson;
    bson_init(&uuid_bson);
    
    Err error = _serialize_content_uuid_bson(uuid, MONGO_THE_ID, 0, &uuid_bson);
    str = bson_as_canonical_extended_json (&uuid_bson, NULL);
    strcpy(buf, str);

    printf("str: %s\n", str);

    bson_free (str);

    bson_destroy(&uuid_bson);

    EXPECT_EQ(S_OK, error);
    EXPECT_STREQ("{ \"the_id\" : \"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\" }", buf);
}

TEST(pttdb, gen_uuid) {
    UUID uuid;
    _UUID _uuid;
    time64_t milli_timestamp;
    time64_t milli_timestamp2;

    // 2018-01-01
    time64_t START_MILLI_TIMESTAMP = 1514764800000;

    // 2018-02-01
    time64_t END_MILLI_TIMESTAMP = 1517443200000;

    gen_uuid(uuid);
    uuid_to_milli_timestamp(uuid, &milli_timestamp);

    EXPECT_GE(milli_timestamp, START_MILLI_TIMESTAMP);
    EXPECT_LT(milli_timestamp, END_MILLI_TIMESTAMP);

    b64_pton((char *)uuid, _uuid, _UUIDLEN);
    EXPECT_EQ(0x60, _uuid[6] & 0xf0);

    gen_uuid(uuid);
    uuid_to_milli_timestamp(uuid, &milli_timestamp2);

    EXPECT_GE(milli_timestamp2, START_MILLI_TIMESTAMP);
    EXPECT_LT(milli_timestamp2, END_MILLI_TIMESTAMP);
    EXPECT_GE(milli_timestamp2, milli_timestamp);

    b64_pton((char *)uuid, _uuid, _UUIDLEN);
    EXPECT_EQ(0x60, _uuid[6] & 0xf0);
}

TEST(pttdb, db_set_if_not_exists) {
    Err error;
    Err error2;
    _UUID _uuid;
    UUID uuid;

    init_mongo_global();
    init_mongo_collections();

    bzero(_uuid, sizeof(_UUID));
    b64_ntop(_uuid, _UUIDLEN, (char *)uuid, UUIDLEN);

    bson_t uuid_bson;
    bson_init(&uuid_bson);
    
    _serialize_content_uuid_bson(uuid, MONGO_THE_ID, 0, &uuid_bson);

    error = db_set_if_not_exists(MONGO_TEST, &uuid_bson);
    error2 = db_set_if_not_exists(MONGO_TEST, &uuid_bson);

    bson_destroy(&uuid_bson);

    free_mongo_collections();
    free_mongo_global();

    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(S_ERR_ALREADY_EXISTS, error2);
}