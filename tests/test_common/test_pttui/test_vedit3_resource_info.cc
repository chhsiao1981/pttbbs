#include "gtest/gtest.h"
#include "test.h"
#include "cmpttui/vedit3_resource_info.h"
#include "cmpttui/vedit3_resource_dict.h"
#include "cmpttui/vedit3_buffer.h"
#include "cmpttui/vedit3_buffer_private.h"
#include "cmmigrate_pttdb.h"
#include "cmpttdb.h"

TEST(vedit3_resource_info, vedit3_resource_info_to_resource_dict)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    error = construct_file_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(main_id, file_info.main_id, UUIDLEN));
    EXPECT_STREQ((char *)"poster0", file_info.main_poster);
    EXPECT_EQ(1511576360000L, file_info.main_create_milli_timestamp);
    EXPECT_EQ(1511576360000L, file_info.main_update_milli_timestamp);
    EXPECT_EQ(41, file_info.n_main_line);
    EXPECT_EQ(1, file_info.n_main_block);
    EXPECT_EQ(103, file_info.n_comment);
    EXPECT_EQ(100, file_info.comments[89].n_comment_reply_total_line);
    EXPECT_EQ(2, file_info.comments[89].n_comment_reply_block);
    EXPECT_EQ(80, file_info.comments[89].comment_reply_blocks[0].n_line);
    EXPECT_EQ(20, file_info.comments[89].comment_reply_blocks[1].n_line);

    // VEdit3
    VEdit3State state = {};
    VEdit3BufferInfo buffer_info = {};

    memcpy(state.top_line_id, file_info.main_content_id, UUIDLEN);
    memcpy(state.main_id, file_info.main_id, UUIDLEN);

    state.top_line_content_type = PTTDB_CONTENT_TYPE_MAIN;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 30;
    state.top_line_comment_offset = 0;

    error = _sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info(&state, &file_info, &buffer_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(1, buffer_info.n_buffer);
    EXPECT_EQ(buffer_info.head, buffer_info.tail);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, buffer_info.head->content_type);
    EXPECT_EQ(NULL, buffer_info.head->next);
    EXPECT_EQ(NULL, buffer_info.head->pre);
    EXPECT_EQ(0, buffer_info.head->block_offset);
    EXPECT_EQ(30, buffer_info.head->line_offset);
    EXPECT_EQ(0, buffer_info.head->comment_offset);

    error = _sync_vedit3_buffer_info_extend_next_buffer_no_buf(&buffer_info, &file_info, HARD_N_VEDIT3_BUFFER);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(214, buffer_info.n_buffer);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, buffer_info.head->content_type);
    EXPECT_EQ(NULL, buffer_info.head->pre);
    EXPECT_EQ(NULL, buffer_info.tail->next);
    EXPECT_EQ(0, buffer_info.head->block_offset);
    EXPECT_EQ(30, buffer_info.head->line_offset);
    EXPECT_EQ(0, buffer_info.head->comment_offset);

    VEdit3Buffer *p = buffer_info.head;
    for(int i = 30; i < 41; i++, p = p->next) {
        EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, p->content_type);
        EXPECT_EQ(0, p->block_offset);
        EXPECT_EQ(i, p->line_offset);
        EXPECT_EQ(0, p->comment_offset);
    }
    for(int i = 0; i < 90; i++, p = p->next) {
        EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT, p->content_type);
        EXPECT_EQ(0, p->block_offset);
        EXPECT_EQ(0, p->line_offset);
        EXPECT_EQ(i, p->comment_offset);
    }
    for(int i = 0; i < 80; i++, p = p->next) {
        EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT_REPLY, p->content_type);
        EXPECT_EQ(0, p->block_offset);
        EXPECT_EQ(i, p->line_offset);
        EXPECT_EQ(89, p->comment_offset);
    }
    for(int i = 0; i < 20; i++, p = p->next) {
        EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT_REPLY, p->content_type);
        EXPECT_EQ(1, p->block_offset);
        EXPECT_EQ(i, p->line_offset);
        EXPECT_EQ(89, p->comment_offset);
    }
    for(int i = 90; i < 103; i++, p = p->next) {
        EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT, p->content_type);
        EXPECT_EQ(0, p->block_offset);
        EXPECT_EQ(0, p->line_offset);
        EXPECT_EQ(i, p->comment_offset);
    }
    EXPECT_EQ(NULL, p);

    // resource-info
    VEdit3ResourceInfo resource_info = {};
    error = _vedit3_buffer_info_to_resource_info(buffer_info.head, &resource_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(1, resource_info.queue[PTTDB_CONTENT_TYPE_MAIN * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO].n_queue);
    EXPECT_EQ(103, resource_info.queue[PTTDB_CONTENT_TYPE_COMMENT * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO].n_queue);
    EXPECT_EQ(2, resource_info.queue[PTTDB_CONTENT_TYPE_COMMENT_REPLY * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO].n_queue);

    // resource-dict
    VEdit3ResourceDict resource_dict = {};
    error = vedit3_resource_info_to_resource_dict(&resource_info, &resource_dict);
    EXPECT_EQ(S_OK, error);

    UUID the_id = {};
    int len = 0;
    char *buf = NULL;
    p = buffer_info.head;
    for(int i = 30; i < 41; i++, p = p->next) {
        memcpy(the_id, p->the_id, UUIDLEN);
        error = vedit3_resource_dict_get_data(&resource_dict, the_id, p->block_offset, &len, &buf);
        EXPECT_EQ(S_OK, error);
        EXPECT_NE(0, len);
        EXPECT_NE((char *)NULL, (char *)buf);
    }
    for(int i = 0; i < 90; i++, p = p->next) {
        memcpy(the_id, p->the_id, UUIDLEN);
        error = vedit3_resource_dict_get_data(&resource_dict, the_id, p->block_offset, &len, &buf);
        EXPECT_EQ(S_OK, error);
        EXPECT_NE(0, len);
        EXPECT_NE((char *)NULL, (char *)buf);
    }
    for(int i = 0; i < 80; i++, p = p->next) {
        memcpy(the_id, p->the_id, UUIDLEN);
        error = vedit3_resource_dict_get_data(&resource_dict, the_id, p->block_offset, &len, &buf);
        EXPECT_EQ(S_OK, error);
        EXPECT_NE(0, len);
        EXPECT_NE((char *)NULL, (char *)buf);
    }
    for(int i = 0; i < 20; i++, p = p->next) {
        memcpy(the_id, p->the_id, UUIDLEN);
        error = vedit3_resource_dict_get_data(&resource_dict, the_id, p->block_offset, &len, &buf);
        EXPECT_EQ(S_OK, error);
        EXPECT_NE(0, len);
        EXPECT_NE((char *)NULL, (char *)buf);
    }
    for(int i = 90; i < 103; i++, p = p->next) {
        memcpy(the_id, p->the_id, UUIDLEN);
        error = vedit3_resource_dict_get_data(&resource_dict, the_id, p->block_offset, &len, &buf);
        EXPECT_EQ(S_OK, error);
        EXPECT_NE(0, len);
        EXPECT_NE((char *)NULL, (char *)buf);
    }

    // free
    safe_destroy_vedit3_resource_dict(&resource_dict);

    destroy_vedit3_buffer_info(&buffer_info);

    destroy_vedit3_resource_info(&resource_info);

    destroy_file_info(&file_info);
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

    pttui_thread_lock_init();

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

    FD = open("log.test_vedit3_resource_info.err", O_WRONLY | O_CREAT | O_TRUNC, 0660);
    dup2(FD, 2);

}

void MyEnvironment::TearDown() {
    free_mongo_collections();
    free_mongo_global();
    pttui_thread_lock_destroy();

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

