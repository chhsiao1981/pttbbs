#include "gtest/gtest.h"
#include "bbs.h"
#include "ptterr.h"
#include "util_db.h"
#include "util_db_internal.h"

TEST(util_db, db_set_if_not_exists) {
    Err error;
    Err error2;

    bson_t b;
    bson_init(&b);

    bson_append_int32(&b, "test", -1, 1);

    error = db_set_if_not_exists(MONGO_TEST, &b);
    fprintf(stderr, "after db_set_if_not_exists: error: %d\n", error);
    EXPECT_EQ(S_OK, error);

    if(error != S_OK) {
        bson_destroy(&b);
        return;
    }

    error2 = db_set_if_not_exists(MONGO_TEST, &b);
    EXPECT_EQ(S_ERR_ALREADY_EXISTS, error2);
    if(error2 != S_ERR_ALREADY_EXISTS) {
        bson_destroy(&b);
        return;
    }

    _DB_FORCE_DROP_COLLECTION(MONGO_TEST);

    bson_destroy(&b);

}

TEST(util_db, db_update_one) {
    Err error;

    bson_t key;
    bson_t val;
    bson_init(&key);
    bson_init(&val);

    bson_append_utf8(&key, "the_key", -1, "key0", 4);
    bson_append_utf8(&val, "the_val", -1, "val0", 4);

    error = db_update_one(MONGO_TEST, &key, &val, true);
    EXPECT_EQ(S_OK, error);
    if(error != S_OK) {
        bson_destroy(&key);
        bson_destroy(&val);
        return;
    }

    _DB_FORCE_DROP_COLLECTION(MONGO_TEST);

    bson_destroy(&key);
    bson_destroy(&val);

}

TEST(util_db, db_find_one) {
    Err error;

    bson_t key;
    bson_t val;
    bson_init(&key);
    bson_init(&val);

    bson_append_int32(&key, "the_key", -1, 4);
    bson_append_int32(&val, "the_val", -1, 5);

    error = db_update_one(MONGO_TEST, &key, &val, true);
    EXPECT_EQ(S_OK, error);
    if(error != S_OK) {
        bson_destroy(&key);
        bson_destroy(&val);
        return;
    }

    bson_t *result;
    error = db_find_one(MONGO_TEST, &key, NULL, result);
    EXPECT_EQ(S_OK, error);
    if(error != S_OK) {
        bson_destroy(&key);
        bson_destroy(&val);
        bson_destroy(result);
    }

    int int_result;
    error = bson_get_value_int32(result, "the_key", &int_result);
    EXPECT_EQ(S_OK, error);
    if(error != S_OK) {
        bson_destroy(&key);
        bson_destroy(&val);
        bson_destroy(result);
    }
    EXPECT_EQ(4, int_result);

    _DB_FORCE_DROP_COLLECTION(MONGO_TEST);

    bson_destroy(&key);
    bson_destroy(&val);
    bson_destroy(result);
}

class MyEnvironment: public ::testing::Environment {
public:
    void SetUp();
    void TearDown();
};

void MyEnvironment::SetUp() {
    Err err = S_OK;
    err = init_mongo_global();
    if(err != S_OK) {
        fprintf(stderr, "[ERROR] UNABLE TO init mongo global\n");
        return;
    }
    err = init_mongo_collections();
    if(err != S_OK) {
        fprintf(stderr, "[ERROR] UNABLE TO init mongo collections\n");
        return;
    }
}

void MyEnvironment::TearDown() {
    free_mongo_collections();
    free_mongo_global();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new MyEnvironment);

    int fd = open("test_util_db.log", O_WRONLY|O_CREAT|O_TRUNC, 0660);
    assert(fd >= 0);
    int ret = dup2(fd, 2);
    assert(ret >= 0);

    return RUN_ALL_TESTS();
}
