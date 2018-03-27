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

TEST(migrate_file_to_db, parse_create_milli_timestamp_from_filename) {
    char filename[] = "M.1510537375.A.8B4";
    time64_t create_milli_timestamp = 0;
    Err error =  _parse_create_milli_timestamp_from_filename(filename, &create_milli_timestamp);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1510537375000, create_milli_timestamp);
}

TEST(migrate_file_to_db, is_comment_line_good_bad_arrow_invalid) {
    char line[MAX_BUF_SIZE] = {};
    sprintf(line, "testtest\r\n");

    bool is_valid = false;
    Err error = _is_comment_line_good_bad_arrow(line, MAX_BUF_SIZE, &is_valid, COMMENT_TYPE_GOOD);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_valid);
}

TEST(migrate_file_to_db, is_comment_line_good_bad_arrow_good) {
    char line[MAX_BUF_SIZE] = {};
    sprintf(line, "%s%s " ANSI_COLOR(33) "%s" ANSI_RESET ANSI_COLOR(33) ":%-*s" ANSI_RESET "%s\n", COMMENT_TYPE_ATTR2[COMMENT_TYPE_GOOD], COMMENT_TYPE_ATTR[COMMENT_TYPE_GOOD], "poster001", 80, "test-msg", "02/31");

    bool is_valid = false;
    Err error = _is_comment_line_good_bad_arrow(line, MAX_BUF_SIZE, &is_valid, COMMENT_TYPE_GOOD);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_valid);
}

TEST(migrate_file_to_db, is_comment_line_good_bad_arrow_bad) {
    char line[MAX_BUF_SIZE] = {};
    sprintf(line, "%s%s " ANSI_COLOR(33) "%s" ANSI_RESET ANSI_COLOR(33) ":%-*s" ANSI_RESET "%s\n", COMMENT_TYPE_ATTR2[COMMENT_TYPE_BAD], COMMENT_TYPE_ATTR[COMMENT_TYPE_BAD], "poster001", 80, "test-msg", "02/31");

    bool is_valid = false;
    Err error = _is_comment_line_good_bad_arrow(line, MAX_BUF_SIZE, &is_valid, COMMENT_TYPE_BAD);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_valid);
}

TEST(migrate_file_to_db, is_comment_line_good_bad_arrow_arrow) {
    char line[MAX_BUF_SIZE] = {};
    sprintf(line, "%s%s " ANSI_COLOR(33) "%s" ANSI_RESET ANSI_COLOR(33) ":%-*s" ANSI_RESET "%s\n", COMMENT_TYPE_ATTR2[COMMENT_TYPE_ARROW], COMMENT_TYPE_ATTR[COMMENT_TYPE_ARROW], "poster001", 80, "test-msg", "02/31");

    bool is_valid = false;
    Err error = _is_comment_line_good_bad_arrow(line, MAX_BUF_SIZE, &is_valid, COMMENT_TYPE_ARROW);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_valid);
}

TEST(migrate_file_to_db, is_comment_line_cross) {
    // bbs.c line: 2255 cross_post()
    char line[MAX_BUF_SIZE] = {};
    sprintf(line, "%s " ANSI_COLOR(1;32) "%s" ANSI_COLOR(0;32) COMMENT_CROSS_PREFIX "%s" ANSI_RESET "%*s%s\n", COMMENT_TYPE_ATTR[COMMENT_TYPE_CROSS], "poster001", COMMENT_CROSS_HIDDEN_BOARD, 80, "", "02/31");

    bool is_valid = false;
    Err error = _is_comment_line_cross(line, MAX_BUF_SIZE, &is_valid);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_valid);
}

TEST(migrate_file_to_db, is_comment_line_cross_invalid) {
    // bbs.c line: 2255 cross_post()
    char line[MAX_BUF_SIZE] = {};
    sprintf(line, "%s%s " ANSI_COLOR(33) "%s" ANSI_RESET ANSI_COLOR(33) ":%-*s" ANSI_RESET "%s\n", COMMENT_TYPE_ATTR2[COMMENT_TYPE_ARROW], COMMENT_TYPE_ATTR[COMMENT_TYPE_ARROW], "poster001", 80, "test-msg", "02/31");

    bool is_valid = false;
    Err error = _is_comment_line_cross(line, MAX_BUF_SIZE, &is_valid);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_valid);
}

TEST(migrate_file_to_db, is_comment_line_invalid) {
    char line[MAX_BUF_SIZE] = {};
    sprintf(line, "testtest\r\n");

    bool is_valid = false;
    Err error = _is_comment_line(line, MAX_BUF_SIZE, &is_valid);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_valid);
}

TEST(migrate_file_to_db, is_comment_line_good) {
    char line[MAX_BUF_SIZE] = {};
    sprintf(line, "%s%s " ANSI_COLOR(33) "%s" ANSI_RESET ANSI_COLOR(33) ":%-*s" ANSI_RESET "%s\n", COMMENT_TYPE_ATTR2[COMMENT_TYPE_GOOD], COMMENT_TYPE_ATTR[COMMENT_TYPE_GOOD], "poster001", 80, "test-msg", "02/31");

    bool is_valid = false;
    Err error = _is_comment_line(line, MAX_BUF_SIZE, &is_valid);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_valid);
}

TEST(migrate_file_to_db, is_comment_line_bad) {
    char line[MAX_BUF_SIZE] = {};
    sprintf(line, "%s%s " ANSI_COLOR(33) "%s" ANSI_RESET ANSI_COLOR(33) ":%-*s" ANSI_RESET "%s\n", COMMENT_TYPE_ATTR2[COMMENT_TYPE_BAD], COMMENT_TYPE_ATTR[COMMENT_TYPE_BAD], "poster001", 80, "test-msg", "02/31");

    bool is_valid = false;
    Err error = _is_comment_line(line, MAX_BUF_SIZE, &is_valid);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_valid);
}

TEST(migrate_file_to_db, is_comment_line_arrow) {
    char line[MAX_BUF_SIZE] = {};
    sprintf(line, "%s%s " ANSI_COLOR(33) "%s" ANSI_RESET ANSI_COLOR(33) ":%-*s" ANSI_RESET "%s\n", COMMENT_TYPE_ATTR2[COMMENT_TYPE_ARROW], COMMENT_TYPE_ATTR[COMMENT_TYPE_ARROW], "poster001", 80, "test-msg", "02/31");

    bool is_valid = false;
    Err error = _is_comment_line(line, MAX_BUF_SIZE, &is_valid);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_valid);
}

TEST(migrate_file_to_db, is_comment_line_cross2) {
    char line[MAX_BUF_SIZE] = {};
    sprintf(line, "%s " ANSI_COLOR(1;32) "%s" ANSI_COLOR(0;32) COMMENT_CROSS_PREFIX "%s" ANSI_RESET "%*s%s\n", COMMENT_TYPE_ATTR[COMMENT_TYPE_CROSS], "poster001", COMMENT_CROSS_HIDDEN_BOARD, 80, "", "02/31");

    bool is_valid = false;
    Err error = _is_comment_line(line, MAX_BUF_SIZE, &is_valid);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_valid);
}

TEST(migrate_file_to_db, parse_legacy_file_main_info_1) {
    // M.1510537375.A.8B4
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.1.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1780, legacy_file_info.main_content_len);

    char *buf = (char *)malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.1.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "test_migrate_file_to_db._parse_legacy_file_main_info_1: migrate_file_to_buf: %s\n", buf);

    //free
    close(fd);
    free(buf);
}

TEST(migrate_file_to_db, parse_legacy_file_n_comment_comment_reply_1) {
    // M.1510537375.A.8B4
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.1.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1780, legacy_file_info.main_content_len);

    int n_comment_comment_reply = 0;
    error = _parse_legacy_file_n_comment_comment_reply("data_test/original_msg.1.txt", legacy_file_info.main_content_len, &n_comment_comment_reply);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(4, n_comment_comment_reply);

    //free
}

TEST(migrate_file_to_db, parse_legacy_file_main_info_2) {
    // M.1500464247.A.6AA
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.2.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(109270, legacy_file_info.main_content_len);

    char *buf = (char *)malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.2.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "test_migrate_file_to_db.parse_legacy_file_main_info_2: migrate_file_to_buf: %s\n", buf);

    //free
    close(fd);
    free(buf);
}

TEST(migrate_file_to_db, parse_legacy_file_n_comment_comment_reply_2) {
    // M.1510537375.A.8B4
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.2.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(109270, legacy_file_info.main_content_len);

    int n_comment_comment_reply = 0;
    error = _parse_legacy_file_n_comment_comment_reply("data_test/original_msg.2.txt", legacy_file_info.main_content_len, &n_comment_comment_reply);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(15, n_comment_comment_reply);

    //free
}

TEST(migrate_file_to_db, parse_legacy_file_main_info_3) {
    // M.1503755396.A.49D
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.3.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(6505, legacy_file_info.main_content_len);

    char *buf = (char *)malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.3.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "test_migrate_file_to_db.parse_legacy_file_main_info_3: migrate_file_to_buf: %s\n", buf);

    //free
    close(fd);
    free(buf);
}

TEST(migrate_file_to_db, parse_legacy_file_n_comment_comment_reply_3) {
    // M.1503755396.A.49D
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.3.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(6505, legacy_file_info.main_content_len);

    int n_comment_comment_reply = 0;
    error = _parse_legacy_file_n_comment_comment_reply("data_test/original_msg.3.txt", legacy_file_info.main_content_len, &n_comment_comment_reply);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(79, n_comment_comment_reply);

    //free
}

TEST(migrate_file_to_db, parse_legacy_file_main_info_4) {
    // M.1511576360.A.A15
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.4.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1309, legacy_file_info.main_content_len);

    char *buf = (char *)malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.4.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "test_migrate_file_to_db.parse_legacy_file_main_info_4: migrate_file_to_buf: %s\n", buf);

    //free
    close(fd);
    free(buf);
}

TEST(migrate_file_to_db, parse_legacy_file_n_comment_comment_reply_4) {
    // M.1511576360.A.A15
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.4.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1309, legacy_file_info.main_content_len);

    int n_comment_comment_reply = 0;
    error = _parse_legacy_file_n_comment_comment_reply("data_test/original_msg.4.txt", legacy_file_info.main_content_len, &n_comment_comment_reply);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(103, n_comment_comment_reply);

    //free
}

TEST(migrate_file_to_db, parse_legacy_file_main_info_5) {
    // M.997843374.A
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.5.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(655, legacy_file_info.main_content_len);

    char *buf = (char *)malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.5.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "test_migrate_file_to_db.parse_legacy_file_main_info_5: migrate_file_to_buf: %s\n", buf);

    //free
    close(fd);
    free(buf);
}

TEST(migrate_file_to_db, parse_legacy_file_n_comment_comment_reply_5) {
    // M.997843374.A
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.5.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(655, legacy_file_info.main_content_len);

    int n_comment_comment_reply = 0;
    error = _parse_legacy_file_n_comment_comment_reply("data_test/original_msg.5.txt", legacy_file_info.main_content_len, &n_comment_comment_reply);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(0, n_comment_comment_reply);

    //free
}

TEST(migrate_file_to_db, parse_legacy_file_main_info_6) {
    // M.997841455.A
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.6.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(779, legacy_file_info.main_content_len);

    char *buf = (char *)malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.6.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "test_migrate_file_to_db.parse_legacy_file_main_info_6: migrate_file_to_buf: %s\n", buf);

    //free
    close(fd);
    free(buf);
}

TEST(migrate_file_to_db, parse_legacy_file_n_comment_comment_reply_6) {
    // M.997841455.A
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.6.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(779, legacy_file_info.main_content_len);

    int n_comment_comment_reply = 0;
    error = _parse_legacy_file_n_comment_comment_reply("data_test/original_msg.6.txt", legacy_file_info.main_content_len, &n_comment_comment_reply);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1, n_comment_comment_reply);

    //free
}

TEST(migrate_file_to_db, parse_legacy_file_main_info_7) {
    // M.997841455.A

    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.7.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(781, legacy_file_info.main_content_len);

    char *buf = (char *)malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.7.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "test_migrate_file_to_db.parse_legacy_file_main_info_7: migrate_file_to_buf: %s\n", buf);

    //free
    close(fd);
    free(buf);
}

TEST(migrate_file_to_db, parse_legacy_file_main_info_8) {
    // M.1041489119.A.C28
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.8.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1993, legacy_file_info.main_content_len);

    char *buf = (char *)malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.8.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "test_migrate_file_to_db.parse_legacy_file_main_info_8: migrate_file_to_buf: %s\n", buf);

    //free
    close(fd);
    free(buf);
}

TEST(migrate_file_to_db, parse_legacy_file_main_info_9) {
    // M.1119222611.A.7A9
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.9.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(1031, legacy_file_info.main_content_len);

    char *buf = (char *)malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.9.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "test_migrate_file_to_db.parse_legacy_file_main_info_9: migrate_file_to_buf: %s\n", buf);

    //free
    close(fd);
    free(buf);
}

TEST(migrate_file_to_db, parse_legacy_file_main_info_10) {
    // M.1.A.5CF
    LegacyFileInfo legacy_file_info = {};

    Err error = _parse_legacy_file_main_info("data_test/original_msg.10.txt", &legacy_file_info);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(2789, legacy_file_info.main_content_len);

    char *buf = (char *)malloc(legacy_file_info.main_content_len + 1);
    bzero(buf, legacy_file_info.main_content_len + 1);

    int fd = open("data_test/original_msg.10.txt", O_RDONLY);
    read(fd, buf, legacy_file_info.main_content_len);
    fprintf(stderr, "test_migrate_file_to_Db.parse_legacy_file_main_info_10.migrate_file_to_buf: %s\n", buf);

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
