#include "gtest/gtest.h"
#include "bbs.h"
#include "ptterr.h"
#include "pttdb.h"
#include "pttdb_internal.h"
#include "pttdb_uuid_internal.h"

// 2018-01-01
time64_t START_MILLI_TIMESTAMP = 1514764800000;

// 2019-01-01
time64_t END_MILLI_TIMESTAMP = 1546300800000;

TEST(pttdb_uuid, get_milli_timestamp) {
    time64_t t;
    get_milli_timestamp(&t);

    EXPECT_GE(t, START_MILLI_TIMESTAMP);
    EXPECT_LT(t, END_MILLI_TIMESTAMP);
}

TEST(pttdb_uuid, gen_uuid) {
    UUID uuid;
    //_UUID _uuid;
    UUID uuid2;
    //_UUID _uuid2;
    time64_t milli_timestamp;
    time64_t milli_timestamp2;

    gen_uuid(uuid);
    uuid_to_milli_timestamp(uuid, &milli_timestamp);

    EXPECT_GE(milli_timestamp, START_MILLI_TIMESTAMP);
    EXPECT_LT(milli_timestamp, END_MILLI_TIMESTAMP);

    //b64_pton((char *)uuid, _uuid, _UUIDLEN);
    EXPECT_EQ(0x60, uuid[6] & 0xf0);

    gen_uuid(uuid2);
    uuid_to_milli_timestamp(uuid2, &milli_timestamp2);

    EXPECT_NE(0, strncmp((char *)uuid, (char *)uuid2, UUIDLEN));
    EXPECT_GE(milli_timestamp2, START_MILLI_TIMESTAMP);
    EXPECT_LT(milli_timestamp2, END_MILLI_TIMESTAMP);
    EXPECT_GE(milli_timestamp2, milli_timestamp);

    //b64_pton((char *)uuid2, _uuid2, _UUIDLEN);
    EXPECT_EQ(0x60, uuid2[6] & 0xf0);
}

TEST(pttdb_uuid, gen_uuid_with_db) {
    UUID uuid;
    //_UUID _uuid;
    UUID uuid2;
    //_UUID _uuid2;
    time64_t milli_timestamp;
    time64_t milli_timestamp2;

    _DB_FORCE_DROP_COLLECTION(MONGO_TEST);

    gen_uuid_with_db(MONGO_TEST, uuid);
    uuid_to_milli_timestamp(uuid, &milli_timestamp);

    EXPECT_GE(milli_timestamp, START_MILLI_TIMESTAMP);
    EXPECT_LT(milli_timestamp, END_MILLI_TIMESTAMP);

    //b64_pton((char *)uuid, _uuid, _UUIDLEN);
    EXPECT_EQ(0x60, uuid[6] & 0xf0);

    gen_uuid_with_db(MONGO_TEST, uuid2);
    uuid_to_milli_timestamp(uuid2, &milli_timestamp2);

    EXPECT_GE(milli_timestamp2, START_MILLI_TIMESTAMP);
    EXPECT_LT(milli_timestamp2, END_MILLI_TIMESTAMP);
    EXPECT_GE(milli_timestamp2, milli_timestamp);

    //b64_pton((char *)uuid2, _uuid2, _UUIDLEN);
    EXPECT_EQ(0x60, uuid2[6] & 0xf0);

    EXPECT_NE(0, strncmp((char *)uuid, (char *)uuid2, UUIDLEN));

    EXPECT_NE(0, strncmp((char *)uuid, (char *)uuid2, UUIDLEN));
}

TEST(pttdb_uuid, serialize_uuid_bson) {
    //_UUID _uuid;
    UUID uuid;
    char *str;
    char buf[MAX_BUF_SIZE];

    bzero(uuid, sizeof(UUID));
    //b64_ntop(_uuid, _UUIDLEN, (char *)uuid, UUIDLEN);

    bson_t *uuid_bson = NULL;

    Err error = _serialize_uuid_bson(uuid, &uuid_bson);
    str = bson_as_canonical_extended_json(uuid_bson, NULL);
    strcpy(buf, str);
    bson_free (str);

    bson_safe_destroy(&uuid_bson);

    fprintf(stderr, "test_pttdb_misc.serialize_uuid_bson: buf: %s\n", buf);

    EXPECT_EQ(S_OK, error);
    EXPECT_STREQ("{ \"the_id\" : { \"$binary\" : { \"base64\": \"AAAAAAAAAAAAAAAAAAAAAA==\", \"subType\" : \"00\" } } }", buf);
}

TEST(pttdb_uuid, serialize_content_uuid_bson) {
    //_UUID _uuid;
    UUID uuid;
    char *str;
    char buf[MAX_BUF_SIZE];

    bzero(uuid, sizeof(UUID));
    //b64_ntop(_uuid, _UUIDLEN, (char *)uuid, UUIDLEN);

    bson_t *uuid_bson;

    Err error = _serialize_content_uuid_bson(uuid, 0, &uuid_bson);
    str = bson_as_canonical_extended_json(uuid_bson, NULL);
    strcpy(buf, str);

    bson_free (str);

    bson_safe_destroy(&uuid_bson);

    EXPECT_EQ(S_OK, error);
    EXPECT_STREQ("{ \"the_id\" : { \"$binary\" : { \"base64\": \"AAAAAAAAAAAAAAAAAAAAAA==\", \"subType\" : \"00\" } }, \"block_id\" : { \"$numberInt\" : \"0\" } }", buf);
}

TEST(pttdb_uuid, uuid_to_milli_timestamp) {
    UUID uuid;
    time64_t milli_timestamp;

    gen_uuid(uuid);
    uuid_to_milli_timestamp(uuid, &milli_timestamp);

    fprintf(stderr, "test_pttdb_misc.uuid_to_milli_timestamp: milli_timestamp: %ld\n", milli_timestamp);

    EXPECT_GE(milli_timestamp, START_MILLI_TIMESTAMP);
    EXPECT_LT(milli_timestamp, END_MILLI_TIMESTAMP);
}

