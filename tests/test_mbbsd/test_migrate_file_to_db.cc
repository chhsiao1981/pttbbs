#include "gtest/gtest.h"
#include "bbs.h"
#include "migrate.h"
#include "migrate_internal.h"
#include "pttdb_internal.h"
#include "util_db_internal.h"

TEST(migrate_file_to_db, parse_create_milli_timestamp_from_web_link) {
    char web_link[] = "https://www.ptt.cc/bbs/SYSOP/M.1510537375.A.8B4.html";

    time64_t create_milli_timestamp = 0;
    Err error = _parse_create_milli_timestamp_from_web_link(web_link, &create_milli_timestamp);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1510537375000, create_milli_timestamp);
}

TEST(migrate_file_to_db, parse_legacy_file_main_info_1) {

    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.1.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1780, legacy_file_info.main_content_len);

    char *buf = malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.1.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "migrate_file_to_buf: %s\n", buf);

    //free
    close(fd);
    free(buf);
}


TEST(migrate_file_to_db, parse_legacy_file_main_info_2) {

    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.2.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1780, legacy_file_info.main_content_len);

    char *buf = malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.2.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "migrate_file_to_buf: %s\n", buf);

    //free
    close(fd);
    free(buf);
}

/**********
 * MAIN
 */
int FD = 0;

class MyEnvironment: public ::testing::Environment {
public:
    void SetUp();
    void TearDown();
};

void MyEnvironment::SetUp() {
    Err err = S_OK;
    FD = open("log.test_migrate_file_to_db.err", O_WRONLY | O_CREAT | O_TRUNC, 0660);
    dup2(FD, 2);

    const char *db_name[] = {
        "test_post",
        "test",
    };

    err = init_mongo_global();
    if (err != S_OK) {
        fprintf(stderr, "[ERROR] UNABLE TO init mongo global\n");
        return;
    }
    err = init_mongo_collections(db_name);
    if (err != S_OK) {
        fprintf(stderr, "[ERROR] UNABLE TO init mongo collections\n");
        return;
    }
}

void MyEnvironment::TearDown() {
    free_mongo_collections();
    free_mongo_global();

    if (FD) {
        close(FD);
        FD = 0;
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new MyEnvironment);

    return RUN_ALL_TESTS();
}
