#include "pttdb.h"
#include "pttdb_internal.h"

/**********
 * Milli-timestamp
 **********/

/**
 * @brief Get current time in milli-timestamp
 * @details Get current time in milli-timestamp
 *
 * @param milli_timestamp milli-timestamp (to-compute)
 * @return Err
 */
Err
get_milli_timestamp(time64_t *milli_timestamp)
{
    struct timeval tv;
    struct timezone tz;

    int ret_code = gettimeofday(&tv, &tz);
    if (ret_code) return S_ERR;

    *milli_timestamp = ((time64_t)tv.tv_sec) * 1000L + ((time64_t)tv.tv_usec) / 1000L;

    return S_OK;
}

/**********
 * UUID
 **********/

/**
 * @brief Generate customized uuid (maybe we can use libuuid to save some random thing.)
 * @details Generate customized uuid (ASSUMING little endian in the system):
 *          1. byte[0-6]: random
 *          2. byte[7]: 0x60 as version
 *          3. byte[8-9]: random
 *          4: byte[10-15]: milli-timestamp
 *
 * @param uuid [description]
 */
Err
gen_uuid(UUID uuid)
{
    Err error_code;

    time64_t milli_timestamp;
    time64_t *p_milli_timestamp;

    // RAND_MAX is 2147483647

    int *p_rand;
    const int n_random = (UUIDLEN - 8) / 4;

    long int rand_num;
    unsigned short *p_short_rand_num;

    unsigned short *p_short;
    unsigned char *p_char;
    // _UUID _uuid;

    // last 8 chars as milli-timestamp, but only the last 6 chars will be used. little-endian
    error_code = get_milli_timestamp(&milli_timestamp);
    if (error_code) return error_code;

    milli_timestamp <<= 16;
    p_char = (unsigned char *)&milli_timestamp;

    p_milli_timestamp = (time64_t *)(uuid + UUIDLEN - 8);
    *p_milli_timestamp = milli_timestamp;

    rand_num = random();
    p_short_rand_num = (unsigned short *)&rand_num;
    p_short = (unsigned short*)(uuid + UUIDLEN - 8);
    *p_short = *p_short_rand_num;

    // first 16 chars as random, but 6th char is version (6 for now)
    p_rand = (int *)uuid;
    for (int i = 0; i < n_random; i++) {
        rand_num = random();
        *p_rand = (int)rand_num;
        p_rand++;
    }

    uuid[6] &= 0x0f;
    uuid[6] |= 0x60;

    // b64_ntop(_uuid, _UUIDLEN, (char *)uuid, UUIDLEN);

    return S_OK;
}

/**
 * @brief Gen uuid and check with db.
 * @details Gen uuid and check with db for uniqueness.
 *
 * @param collection Collection
 * @param uuid uuid (to-compute).
 * @return Err
 */
Err
gen_uuid_with_db(int collection, UUID uuid)
{
    Err error_code = S_OK;
    bson_t *uuid_bson = NULL;

    for (int i = 0; i < N_GEN_UUID_WITH_DB; i++) {
        error_code = gen_uuid(uuid);

        if(!error_code) {
            error_code = _serialize_uuid_bson(uuid, &uuid_bson);
        }

        if(!error_code) {
            error_code = db_set_if_not_exists(collection, uuid_bson);
        }

        bson_safe_destroy(&uuid_bson);

        if (!error_code) return S_OK;
    }

    return error_code;
}

/**
 * @brief Gen uuid with block-id as 0 and check with db.
 * @details Gen uuid and check with db for uniqueness.
 *
 * @param collection Collection
 * @param uuid uuid (to-compute).
 * @return Err
 */
Err
gen_content_uuid_with_db(int collection, UUID uuid)
{
    Err error_code = S_OK;
    bson_t *uuid_bson = NULL;

    for (int i = 0; i < N_GEN_UUID_WITH_DB; i++) {
        error_code = gen_uuid(uuid);

        if(!error_code) {
            error_code = _serialize_content_uuid_bson(uuid, 0, &uuid_bson);
        }

        if(!error_code) {
            error_code = db_set_if_not_exists(collection, uuid_bson);
        }

        bson_safe_destroy(&uuid_bson);

        if (!error_code) return S_OK;
    }

    return error_code;
}

/**
 * @brief Serialize uuid to bson
 * @details Serialize uuid to bson, and name the uuid as the_id.
 *
 * @param uuid uuid
 * @param db_set_bson bson (to-compute)
 * @return Err
 */
Err
_serialize_uuid_bson(UUID uuid, bson_t **uuid_bson)
{
    *uuid_bson = BCON_NEW(
        MONGO_THE_ID, BCON_BINARY(uuid, UUIDLEN)
        );

    return S_OK;
}

/**
 * @brief Serialize uuid to bson
 * @details Serialize uuid to bson, and name the uuid as the_id.
 *
 * @param uuid uuid
 * @param db_set_bson bson (to-compute)
 * @return Err
 */
Err
_serialize_content_uuid_bson(UUID uuid, int block_id, bson_t **uuid_bson)
{
    *uuid_bson = BCON_NEW(
        MONGO_THE_ID, BCON_BINARY(uuid, UUIDLEN),
        MONGO_BLOCK_ID, BCON_INT32(block_id)
        );

    return S_OK;
}

Err
uuid_to_milli_timestamp(UUID uuid, time64_t *milli_timestamp)
{
    //_UUID _uuid;
    time64_t *p_uuid = (time64_t *)(uuid + UUIDLEN - 8);
    //b64_pton((char *)uuid, _uuid, _UUIDLEN);

    *milli_timestamp = *p_uuid;
    *milli_timestamp >>= 16;

    return S_OK;
}

/**********
 * Get line from buf
 **********/

/**
 * @brief Try to get a line (ending with \r\n) from buffer.
 * @details [long description]
 *
 * @param p_buf Starting point of the buffer.
 * @param offset_buf Current offset of the p_buf in the whole buffer.
 * @param bytes Total bytes of the buffer.
 * @param p_line Starting point of the line.
 * @param offset_line Offset of the line.
 * @param bytes_in_new_line To be obtained bytes in new extracted line.
 * @return Error
 */
Err
get_line_from_buf(char *p_buf, int offset_buf, int bytes, char *p_line, int offset_line, int *bytes_in_new_line)
{

    // check the end of buf
    if (offset_buf >= bytes) {
        *bytes_in_new_line = 0;

        return S_ERR;
    }

    // init p_buf offset
    p_buf += offset_buf;
    p_line += offset_line;

    // check bytes in line and in buf.
    if (offset_line && p_line[-1] == '\r' && p_buf[0] == '\n') {
        *p_line = '\n';
        *bytes_in_new_line = 1;

        return S_OK;
    }

    // check \r\n in buf.
    int max_new_lines = MAX_BUF_SIZE - offset_line;
    int iter_bytes = (bytes - offset_buf <= max_new_lines) ? (bytes - offset_buf) : max_new_lines;
    int end_bytes = iter_bytes + offset_buf;
    for (int i = offset_buf; i < end_bytes - 1; i++) {
        if (*p_buf == '\r' && *(p_buf + 1) == '\n') {
            *p_line = '\r';
            *(p_line + 1) = '\n';
            *bytes_in_new_line = i - offset_buf + 1 + 1;

            return S_OK;
        }

        *p_line++ = *p_buf++;
    }

    // last char
    *p_line++ = *p_buf++;
    *bytes_in_new_line = end_bytes - offset_buf;

    // XXX special case for all block as a continuous string. Although it's not end yet, it forms a block.
    if(*bytes_in_new_line == max_new_lines) return S_OK;

    return S_ERR;
}

Err
pttdb_count_lines(char *content, int len, int *n_line)
{
    int tmp_n_line = 0;
    char *p_content = content;
    for(int i = 0; i < len - 1; i++, p_content++) {
        if(*p_content == '\r' && *(p_content + 1) == '\n') {
            tmp_n_line++;
        }
    }
    *n_line = tmp_n_line;
    fprintf(stderr, "pttdb_misc.pttdb_count_lines: content: %s n_line: %d\n", content, tmp_n_line);

    return S_OK;
}

Err
safe_free_b_list(bson_t ***b, int n)
{
    bson_t **p_b = *b;
    for(int i = 0; i < n; i++, p_b++) {
        bson_safe_destroy(p_b);
    }
    safe_free((void **)b);

    return S_OK;
}

Err
safe_free(void **a)
{
    if(!(*a)) return S_OK;
    
    free(*a);
    *a = NULL;
    return S_OK;
}

char *
_display_uuid(UUID uuid)
{
    char *result = malloc(DISPLAY_UUIDLEN + 1);
    result[DISPLAY_UUIDLEN] = 0;

    b64_ntop(uuid, UUIDLEN, (char *)result, DISPLAY_UUIDLEN);

    return result;
}
