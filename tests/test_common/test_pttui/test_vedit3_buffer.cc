#include "gtest/gtest.h"
#include "test.h"
#include "cmpttui/vedit3_buffer.h"
#include "cmpttui/vedit3_buffer_private.h"
#include "cmmigrate_pttdb.h"
#include "cmpttdb.h"

TEST(vedit3_buffer, vedit3_buffer_is_end_ne)
{
    VEdit3Buffer buffer = {};
    buffer.is_to_delete = true;

    EXPECT_EQ(true, vedit3_buffer_is_end_ne(&buffer));

    buffer.is_to_delete = false;

    EXPECT_EQ(false, vedit3_buffer_is_end_ne(&buffer));
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre2)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_MAIN;
    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre3)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_MAIN;
    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre4)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT;
    buffer.content_type = PTTDB_CONTENT_TYPE_MAIN;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre5)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    buffer.content_type = PTTDB_CONTENT_TYPE_MAIN;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre6)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_MAIN;
    state.top_line_block_offset = 0;
    buffer.content_type = PTTDB_CONTENT_TYPE_MAIN;
    buffer.block_offset = 1;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre7)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_MAIN;
    state.top_line_block_offset = 1;
    buffer.content_type = PTTDB_CONTENT_TYPE_MAIN;
    buffer.block_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre8)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_MAIN;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 0;
    buffer.content_type = PTTDB_CONTENT_TYPE_MAIN;
    buffer.block_offset = 0;
    buffer.line_offset = 1;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre9)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_MAIN;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 1;
    buffer.content_type = PTTDB_CONTENT_TYPE_MAIN;
    buffer.block_offset = 0;
    buffer.line_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre10)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_MAIN;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 0;
    buffer.content_type = PTTDB_CONTENT_TYPE_MAIN;
    buffer.block_offset = 0;
    buffer.line_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre11)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 0;
    state.top_line_comment_offset = 0;

    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT;
    buffer.block_offset = 0;
    buffer.line_offset = 0;
    buffer.comment_offset = 1;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre12)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 0;
    state.top_line_comment_offset = 1;

    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    buffer.block_offset = 0;
    buffer.line_offset = 0;
    buffer.comment_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre13)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 0;
    state.top_line_comment_offset = 0;

    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    buffer.block_offset = 0;
    buffer.line_offset = 0;
    buffer.comment_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre14)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 0;
    state.top_line_comment_offset = 0;

    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT;
    buffer.block_offset = 0;
    buffer.line_offset = 0;
    buffer.comment_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre15)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 0;
    state.top_line_comment_offset = 0;

    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    buffer.block_offset = 1;
    buffer.line_offset = 0;
    buffer.comment_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre16)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    state.top_line_block_offset = 1;
    state.top_line_line_offset = 0;
    state.top_line_comment_offset = 0;

    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    buffer.block_offset = 0;
    buffer.line_offset = 0;
    buffer.comment_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre17)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 0;
    state.top_line_comment_offset = 0;

    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    buffer.block_offset = 0;
    buffer.line_offset = 1;
    buffer.comment_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(true, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre18)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 1;
    state.top_line_comment_offset = 0;

    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    buffer.block_offset = 0;
    buffer.line_offset = 0;
    buffer.comment_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}


TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre19)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 0;
    state.top_line_comment_offset = 0;

    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;
    buffer.block_offset = 0;
    buffer.line_offset = 0;
    buffer.comment_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_is_pre20)
{

    VEdit3State state = {};
    VEdit3Buffer buffer = {};
    bool is_pre = false;

    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 0;
    state.top_line_comment_offset = 0;

    buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT;
    buffer.block_offset = 0;
    buffer.line_offset = 0;
    buffer.comment_offset = 0;

    Err error = _sync_vedit3_buffer_info_is_pre(&state, &buffer, &is_pre);
    EXPECT_EQ(S_OK, error);
    EXPECT_EQ(false, is_pre);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after db force drop\n");

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to sync\n");
    error = _sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info(&state, &file_info, &buffer_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after sync\n");
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(1, buffer_info.n_buffer);
    EXPECT_EQ(buffer_info.head, buffer_info.tail);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, buffer_info.head->content_type);
    EXPECT_EQ(0, buffer_info.head->block_offset);
    EXPECT_EQ(0, buffer_info.head->line_offset);
    EXPECT_EQ(0, buffer_info.head->comment_offset);
    EXPECT_EQ(PTTDB_STORAGE_TYPE_MONGO, buffer_info.head->storage_type);
    EXPECT_EQ(0, buffer_info.head->len);
    EXPECT_EQ(NULL, buffer_info.head->buf);
    EXPECT_EQ(0, memcmp(main_id, buffer_info.main_id, UUIDLEN));
    EXPECT_EQ(0, memcmp(file_info.main_content_id, buffer_info.head->the_id, UUIDLEN));

    // free

    error = destroy_file_info(&file_info);
    EXPECT_EQ(S_OK, error);

    error = destroy_vedit3_buffer_info(&buffer_info);
    EXPECT_EQ(S_OK, error);

}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer_no_buf_main)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after db force drop\n");

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_main(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(NULL, new_buffer);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer_no_buf_main2)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after db force drop\n");

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.line_offset = 3;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_main(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(new_buffer->the_id, current_buffer.the_id, UUIDLEN));
    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(2, new_buffer->line_offset);
    EXPECT_EQ(0, new_buffer->comment_offset);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer_no_buf_main3)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1500464247.A.6AA";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.2.txt", (char *)"poster0", (char *)"BBSmovie", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/BBSmovie/M.1500464247.A.6AA.html", 1500464247000L, main_id);

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    error = construct_file_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(main_id, file_info.main_id, UUIDLEN));
    EXPECT_STREQ((char *)"poster0", file_info.main_poster);
    EXPECT_EQ(1500464247000L, file_info.main_create_milli_timestamp);
    EXPECT_EQ(1500464247000L, file_info.main_update_milli_timestamp);
    EXPECT_EQ(2124, file_info.n_main_line);
    EXPECT_EQ(16, file_info.n_main_block);
    EXPECT_EQ(15, file_info.n_comment);
    EXPECT_EQ(3, file_info.comments[0].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[0].n_comment_reply_block);
    EXPECT_EQ(3, file_info.comments[0].comment_reply_blocks[0].n_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_block);
    EXPECT_EQ(1, file_info.comments[11].comment_reply_blocks[0].n_line);

    // VEdit3
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 2;
    current_buffer.line_offset = 0;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_main(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(new_buffer->the_id, current_buffer.the_id, UUIDLEN));
    EXPECT_EQ(1, new_buffer->block_offset);
    EXPECT_EQ(255, new_buffer->line_offset);
    EXPECT_EQ(0, new_buffer->comment_offset);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1500464247.A.6AA";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.2.txt", (char *)"poster0", (char *)"BBSmovie", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/BBSmovie/M.1500464247.A.6AA.html", 1500464247000L, main_id);

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    error = construct_file_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(main_id, file_info.main_id, UUIDLEN));
    EXPECT_STREQ((char *)"poster0", file_info.main_poster);
    EXPECT_EQ(1500464247000L, file_info.main_create_milli_timestamp);
    EXPECT_EQ(1500464247000L, file_info.main_update_milli_timestamp);
    EXPECT_EQ(2124, file_info.n_main_line);
    EXPECT_EQ(16, file_info.n_main_block);
    EXPECT_EQ(15, file_info.n_comment);
    EXPECT_EQ(3, file_info.comments[0].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[0].n_comment_reply_block);
    EXPECT_EQ(3, file_info.comments[0].comment_reply_blocks[0].n_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_block);
    EXPECT_EQ(1, file_info.comments[11].comment_reply_blocks[0].n_line);

    // VEdit3
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 0;
    current_buffer.comment_offset = 0;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(new_buffer->the_id, file_info.main_content_id, UUIDLEN));
    EXPECT_EQ(15, new_buffer->block_offset);
    EXPECT_EQ(62, new_buffer->line_offset);
    EXPECT_EQ(0, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment2)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1500464247.A.6AA";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.2.txt", (char *)"poster0", (char *)"BBSmovie", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/BBSmovie/M.1500464247.A.6AA.html", 1500464247000L, main_id);

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    error = construct_file_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(main_id, file_info.main_id, UUIDLEN));
    EXPECT_STREQ((char *)"poster0", file_info.main_poster);
    EXPECT_EQ(1500464247000L, file_info.main_create_milli_timestamp);
    EXPECT_EQ(1500464247000L, file_info.main_update_milli_timestamp);
    EXPECT_EQ(2124, file_info.n_main_line);
    EXPECT_EQ(16, file_info.n_main_block);
    EXPECT_EQ(15, file_info.n_comment);
    EXPECT_EQ(3, file_info.comments[0].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[0].n_comment_reply_block);
    EXPECT_EQ(3, file_info.comments[0].comment_reply_blocks[0].n_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_block);
    EXPECT_EQ(1, file_info.comments[11].comment_reply_blocks[0].n_line);

    // VEdit3
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 0;
    current_buffer.comment_offset = 1;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(new_buffer->the_id, file_info.comments[0].comment_reply_id, UUIDLEN));
    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(2, new_buffer->line_offset);
    EXPECT_EQ(0, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT_REPLY, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment3)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1500464247.A.6AA";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.2.txt", (char *)"poster0", (char *)"BBSmovie", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/BBSmovie/M.1500464247.A.6AA.html", 1500464247000L, main_id);

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    error = construct_file_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(main_id, file_info.main_id, UUIDLEN));
    EXPECT_STREQ((char *)"poster0", file_info.main_poster);
    EXPECT_EQ(1500464247000L, file_info.main_create_milli_timestamp);
    EXPECT_EQ(1500464247000L, file_info.main_update_milli_timestamp);
    EXPECT_EQ(2124, file_info.n_main_line);
    EXPECT_EQ(16, file_info.n_main_block);
    EXPECT_EQ(15, file_info.n_comment);
    EXPECT_EQ(3, file_info.comments[0].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[0].n_comment_reply_block);
    EXPECT_EQ(3, file_info.comments[0].comment_reply_blocks[0].n_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_block);
    EXPECT_EQ(1, file_info.comments[11].comment_reply_blocks[0].n_line);

    // VEdit3
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 0;
    current_buffer.comment_offset = 2;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(new_buffer->the_id, file_info.comments[1].comment_id, UUIDLEN));
    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(0, new_buffer->line_offset);
    EXPECT_EQ(1, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1500464247.A.6AA";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.2.txt", (char *)"poster0", (char *)"BBSmovie", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/BBSmovie/M.1500464247.A.6AA.html", 1500464247000L, main_id);

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    error = construct_file_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(main_id, file_info.main_id, UUIDLEN));
    EXPECT_STREQ((char *)"poster0", file_info.main_poster);
    EXPECT_EQ(1500464247000L, file_info.main_create_milli_timestamp);
    EXPECT_EQ(1500464247000L, file_info.main_update_milli_timestamp);
    EXPECT_EQ(2124, file_info.n_main_line);
    EXPECT_EQ(16, file_info.n_main_block);
    EXPECT_EQ(15, file_info.n_comment);
    EXPECT_EQ(3, file_info.comments[0].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[0].n_comment_reply_block);
    EXPECT_EQ(3, file_info.comments[0].comment_reply_blocks[0].n_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_block);
    EXPECT_EQ(1, file_info.comments[11].comment_reply_blocks[0].n_line);

    // VEdit3
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 0;
    current_buffer.comment_offset = 0;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(new_buffer->the_id, file_info.comments[0].comment_id, UUIDLEN));
    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(0, new_buffer->line_offset);
    EXPECT_EQ(0, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply2)
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 0;
    current_buffer.comment_offset = 89;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(new_buffer->the_id, file_info.comments[89].comment_id, UUIDLEN));
    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(0, new_buffer->line_offset);
    EXPECT_EQ(89, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply3)
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.comments[89].comment_reply_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 1;
    current_buffer.comment_offset = 89;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(new_buffer->the_id, file_info.comments[89].comment_reply_id, UUIDLEN));
    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(0, new_buffer->line_offset);
    EXPECT_EQ(89, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT_REPLY, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}


TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply4)
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.comments[89].comment_reply_id, UUIDLEN);
    current_buffer.block_offset = 1;
    current_buffer.line_offset = 0;
    current_buffer.comment_offset = 89;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_pre_buffer_no_buf_comment_reply(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(new_buffer->the_id, file_info.comments[89].comment_reply_id, UUIDLEN));
    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(79, new_buffer->line_offset);
    EXPECT_EQ(89, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT_REPLY, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}


TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_next_buffer_no_buf_main)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after db force drop\n");

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 0;
    current_buffer.comment_offset = 0;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_MAIN;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_main(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(1, new_buffer->line_offset);
    EXPECT_EQ(0, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_next_buffer_no_buf_main2)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1500464247.A.6AA";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.2.txt", (char *)"poster0", (char *)"BBSmovie", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/BBSmovie/M.1500464247.A.6AA.html", 1500464247000L, main_id);

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    error = construct_file_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(main_id, file_info.main_id, UUIDLEN));
    EXPECT_STREQ((char *)"poster0", file_info.main_poster);
    EXPECT_EQ(1500464247000L, file_info.main_create_milli_timestamp);
    EXPECT_EQ(1500464247000L, file_info.main_update_milli_timestamp);
    EXPECT_EQ(2124, file_info.n_main_line);
    EXPECT_EQ(16, file_info.n_main_block);
    EXPECT_EQ(15, file_info.n_comment);
    EXPECT_EQ(3, file_info.comments[0].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[0].n_comment_reply_block);
    EXPECT_EQ(3, file_info.comments[0].comment_reply_blocks[0].n_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_block);
    EXPECT_EQ(1, file_info.comments[11].comment_reply_blocks[0].n_line);

    // VEdit3
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 255;
    current_buffer.comment_offset = 0;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_MAIN;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_main(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(new_buffer->the_id, file_info.main_content_id, UUIDLEN));
    EXPECT_EQ(1, new_buffer->block_offset);
    EXPECT_EQ(0, new_buffer->line_offset);
    EXPECT_EQ(0, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_next_buffer_no_buf_main3)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1500464247.A.6AA";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.2.txt", (char *)"poster0", (char *)"BBSmovie", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/BBSmovie/M.1500464247.A.6AA.html", 1500464247000L, main_id);

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    error = construct_file_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(main_id, file_info.main_id, UUIDLEN));
    EXPECT_STREQ((char *)"poster0", file_info.main_poster);
    EXPECT_EQ(1500464247000L, file_info.main_create_milli_timestamp);
    EXPECT_EQ(1500464247000L, file_info.main_update_milli_timestamp);
    EXPECT_EQ(2124, file_info.n_main_line);
    EXPECT_EQ(16, file_info.n_main_block);
    EXPECT_EQ(15, file_info.n_comment);
    EXPECT_EQ(3, file_info.comments[0].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[0].n_comment_reply_block);
    EXPECT_EQ(3, file_info.comments[0].comment_reply_blocks[0].n_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_total_line);
    EXPECT_EQ(1, file_info.comments[11].n_comment_reply_block);
    EXPECT_EQ(1, file_info.comments[11].comment_reply_blocks[0].n_line);

    // VEdit3
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 15;
    current_buffer.line_offset = 62;
    current_buffer.comment_offset = 0;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_MAIN;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_main(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(new_buffer->the_id, file_info.comments[0].comment_id, UUIDLEN));
    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(0, new_buffer->line_offset);
    EXPECT_EQ(0, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_next_buffer_no_buf_main4)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.997843374.A";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.5.txt", (char *)"poster0", (char *)"BBSmovie", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/b885060xx/M.997843374.A.html", 997843374000L, main_id);

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    error = construct_file_info(main_id, &file_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, memcmp(main_id, file_info.main_id, UUIDLEN));
    EXPECT_STREQ((char *)"poster0", file_info.main_poster);
    EXPECT_EQ(997843374000L, file_info.main_create_milli_timestamp);
    EXPECT_EQ(997843374000L, file_info.main_update_milli_timestamp);
    EXPECT_EQ(23, file_info.n_main_line);
    EXPECT_EQ(1, file_info.n_main_block);
    EXPECT_EQ(23, file_info.main_blocks[0].n_line);
    EXPECT_EQ(0, file_info.n_comment);

    // VEdit3
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 22;
    current_buffer.comment_offset = 0;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_MAIN;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_main(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(NULL, new_buffer);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 0;
    current_buffer.comment_offset = 0;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(0, new_buffer->line_offset);
    EXPECT_EQ(1, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment2)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 0;
    current_buffer.comment_offset = 89;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(0, new_buffer->line_offset);
    EXPECT_EQ(89, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT_REPLY, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment3)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 0;
    current_buffer.comment_offset = 102;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(NULL, new_buffer);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment_reply)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 0;
    current_buffer.comment_offset = 89;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment_reply(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(1, new_buffer->line_offset);
    EXPECT_EQ(89, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT_REPLY, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment_reply2)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 0;
    current_buffer.line_offset = 79;
    current_buffer.comment_offset = 89;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment_reply(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(1, new_buffer->block_offset);
    EXPECT_EQ(0, new_buffer->line_offset);
    EXPECT_EQ(89, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT_REPLY, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment_reply3)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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
    VEdit3Buffer current_buffer = {};
    memcpy(current_buffer.the_id, file_info.main_content_id, UUIDLEN);
    current_buffer.block_offset = 1;
    current_buffer.line_offset = 19;
    current_buffer.comment_offset = 89;
    current_buffer.content_type = PTTDB_CONTENT_TYPE_COMMENT_REPLY;

    VEdit3Buffer *new_buffer = NULL;

    error = _sync_vedit3_buffer_info_extend_next_buffer_no_buf_comment_reply(&current_buffer, &file_info, &new_buffer);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(0, new_buffer->block_offset);
    EXPECT_EQ(0, new_buffer->line_offset);
    EXPECT_EQ(90, new_buffer->comment_offset);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT, new_buffer->content_type);

    // free
    safe_free((void **)&new_buffer);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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

    error = _sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info(&state, &file_info, &buffer_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(1, buffer_info.n_buffer);
    EXPECT_EQ(buffer_info.head, buffer_info.tail);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, buffer_info.head->content_type);
    EXPECT_EQ(NULL, buffer_info.head->next);
    EXPECT_EQ(NULL, buffer_info.head->pre);
    EXPECT_EQ(0, buffer_info.head->block_offset);
    EXPECT_EQ(0, buffer_info.head->line_offset);
    EXPECT_EQ(0, buffer_info.head->comment_offset);

    error = _sync_vedit3_buffer_info_extend_pre_buffer(&buffer_info, &state, &file_info, HARD_N_VEDIT3_BUFFER);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(1, buffer_info.n_buffer);
    EXPECT_EQ(buffer_info.head, buffer_info.tail);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, buffer_info.head->content_type);
    EXPECT_EQ(NULL, buffer_info.head->next);
    EXPECT_EQ(NULL, buffer_info.head->pre);
    EXPECT_EQ(0, buffer_info.head->block_offset);
    EXPECT_EQ(0, buffer_info.head->line_offset);
    EXPECT_EQ(0, buffer_info.head->comment_offset);

    // free
    destroy_vedit3_buffer_info(&buffer_info);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer2)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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
    state.top_line_line_offset = 40;

    error = _sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info(&state, &file_info, &buffer_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(1, buffer_info.n_buffer);
    EXPECT_EQ(buffer_info.head, buffer_info.tail);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, buffer_info.head->content_type);
    EXPECT_EQ(NULL, buffer_info.head->next);
    EXPECT_EQ(NULL, buffer_info.head->pre);
    EXPECT_EQ(0, buffer_info.head->block_offset);
    EXPECT_EQ(40, buffer_info.head->line_offset);
    EXPECT_EQ(0, buffer_info.head->comment_offset);

    error = _sync_vedit3_buffer_info_extend_pre_buffer(&buffer_info, &state, &file_info, HARD_N_VEDIT3_BUFFER);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(41, buffer_info.n_buffer);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, buffer_info.head->content_type);
    EXPECT_EQ(NULL, buffer_info.head->pre);
    EXPECT_EQ(NULL, buffer_info.tail->next);
    EXPECT_EQ(0, buffer_info.head->block_offset);
    EXPECT_EQ(0, buffer_info.head->line_offset);
    EXPECT_EQ(0, buffer_info.head->comment_offset);

    // free
    destroy_vedit3_buffer_info(&buffer_info);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_pre_buffer3)
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

    memcpy(state.top_line_id, file_info.comments[102].comment_id, UUIDLEN);
    memcpy(state.main_id, file_info.main_id, UUIDLEN);
    state.top_line_content_type = PTTDB_CONTENT_TYPE_COMMENT;
    state.top_line_block_offset = 0;
    state.top_line_line_offset = 0;
    state.top_line_comment_offset = 102;

    error = _sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info(&state, &file_info, &buffer_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(1, buffer_info.n_buffer);
    EXPECT_EQ(buffer_info.head, buffer_info.tail);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_COMMENT, buffer_info.head->content_type);
    EXPECT_EQ(NULL, buffer_info.head->next);
    EXPECT_EQ(NULL, buffer_info.head->pre);
    EXPECT_EQ(0, buffer_info.head->block_offset);
    EXPECT_EQ(0, buffer_info.head->line_offset);
    EXPECT_EQ(102, buffer_info.head->comment_offset);

    error = _sync_vedit3_buffer_info_extend_pre_buffer(&buffer_info, &state, &file_info, HARD_N_VEDIT3_BUFFER);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(244, buffer_info.n_buffer);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, buffer_info.head->content_type);
    EXPECT_EQ(NULL, buffer_info.head->pre);
    EXPECT_EQ(NULL, buffer_info.tail->next);
    EXPECT_EQ(0, buffer_info.head->block_offset);
    EXPECT_EQ(0, buffer_info.head->line_offset);
    EXPECT_EQ(0, buffer_info.head->comment_offset);

    VEdit3Buffer *p = buffer_info.head;
    for(int i = 0; i < 41; i++, p = p->next) {
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

    // free
    destroy_vedit3_buffer_info(&buffer_info);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, sync_vedit3_buffer_info_extend_next_buffer)
{
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN);
    _DB_FORCE_DROP_COLLECTION(MONGO_MAIN_CONTENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY);
    _DB_FORCE_DROP_COLLECTION(MONGO_COMMENT_REPLY_BLOCK);

    char *filename = (char *)"M.1511576360.A.A15";
    aidu_t aid = fn2aidu(filename);

    UUID main_id = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to migrate file to pttdb\n");
    Err error = migrate_file_to_pttdb((const char *)"data_test/original_msg.4.txt", (char *)"poster0", (char *)"Gossiping", (char *)"title1", (char *)"ptt.cc", aid, (char *)"https://www.ptt.cc/bbs/Gossiping/M.1511576360.A.A15.html", 1511576360000L, main_id);

    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after migrate file to pttdb\n");

    EXPECT_EQ(S_OK, error);

    FileInfo file_info = {};
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: to construct file_info\n");
    error = construct_file_info(main_id, &file_info);
    fprintf(stderr, "test_vedit3_buffer.sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info: after construct file_info\n");
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

    error = _sync_vedit3_buffer_info_init_buffer_no_buf_from_file_info(&state, &file_info, &buffer_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(1, buffer_info.n_buffer);
    EXPECT_EQ(buffer_info.head, buffer_info.tail);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, buffer_info.head->content_type);
    EXPECT_EQ(NULL, buffer_info.head->next);
    EXPECT_EQ(NULL, buffer_info.head->pre);
    EXPECT_EQ(0, buffer_info.head->block_offset);
    EXPECT_EQ(0, buffer_info.head->line_offset);
    EXPECT_EQ(0, buffer_info.head->comment_offset);

    error = _sync_vedit3_buffer_info_extend_next_buffer(&buffer_info, &state, &file_info, HARD_N_VEDIT3_BUFFER);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(244, buffer_info.n_buffer);
    EXPECT_EQ(PTTDB_CONTENT_TYPE_MAIN, buffer_info.head->content_type);
    EXPECT_EQ(NULL, buffer_info.head->pre);
    EXPECT_EQ(NULL, buffer_info.tail->next);
    EXPECT_EQ(0, buffer_info.head->block_offset);
    EXPECT_EQ(0, buffer_info.head->line_offset);
    EXPECT_EQ(0, buffer_info.head->comment_offset);

    // free
    destroy_vedit3_buffer_info(&buffer_info);

    destroy_file_info(&file_info);
}

TEST(vedit3_buffer, vedit3_buffer_info_to_resource_info)
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

    fprintf(stderr, "test_vedit3_buffer.vedit3_buffer_info_to_resource_info: to loop buffer_info\n");

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

    fprintf(stderr, "test_vedit3_buffer.vedit3_buffer_info_to_resource_info: after loop buffer_info\n");

    VEdit3ResourceInfo resource_info = {};
    error = _vedit3_buffer_info_to_resource_info(buffer_info.head, &resource_info);
    EXPECT_EQ(S_OK, error);

    EXPECT_EQ(1, resource_info.queue[PTTDB_CONTENT_TYPE_MAIN * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO].n_queue);
    EXPECT_EQ(103, resource_info.queue[PTTDB_CONTENT_TYPE_COMMENT * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO].n_queue);
    EXPECT_EQ(2, resource_info.queue[PTTDB_CONTENT_TYPE_COMMENT_REPLY * N_PTTDB_STORAGE_TYPE + PTTDB_STORAGE_TYPE_MONGO].n_queue);

    // free
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

    FD = open("log.test_vedit3_buffer.err", O_WRONLY | O_CREAT | O_TRUNC, 0660);
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

