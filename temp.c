#include <stdio.h>
#include <mongoc.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUFFER 8192

// Mongo Post
#define MONGO_POST_DBNAME "post"

#define MONGO_MAIN_NAME "main"
#define MONGO_MAIN_CONTENT_NAME "main_content"

// Mongo Test
#define MONGO_TEST_DBNAME "test"

#define MONGO_TEST_NAME "test"

// Mongo the id
#define MONGO_THE_ID "the_id"
#define MONGO_BLOCK_ID "block_id"

enum {
    MONGO_MAIN,
    MONGO_MAIN_CONTENT,

    MONGO_TEST,

    N_MONGO_COLLECTIONS,
};

mongoc_uri_t *MONGO_URI;
mongoc_client_pool_t *MONGO_CLIENT_POOL;

mongoc_client_t *MONGO_CLIENT;
mongoc_collection_t **MONGO_COLLECTIONS;

unsigned long int get_memory_size(pid_t pid) {
    char filename[MAX_BUFFER];
    // man 5 proc
    pid_t pid2;                   // 1
    char comm[MAX_BUFFER];        // 2
    char state;                   // 3
    int ppid;                     // 4
    int pgrp;                     // 5
    int session;                  // 6
    int tty_nr;                   // 7
    int tpgid;                    // 8
    unsigned int flags;           // 9
    unsigned long int minflt;     //10
    unsigned long int cminflt;    //11
    unsigned long int majflt;     //12
    unsigned long int cmajflt;    //13
    unsigned long int utime;      //14
    unsigned long int stime;      //15
    long int cutime;              //16
    long int cstime;              //17
    long int priority;            //18
    long int nice;                //19
    long int num_threads;         //20
    long int itrealvalue;         //21
    unsigned long long int starttime; //22
    unsigned long int vsize;      //23

    sprintf(filename, "/proc/%d/stat", pid);
    FILE *f = fopen(filename, "r" );
    // man 5 proc
    //       1  2  3  4  5  6  7  8  9  10  11  12  13  14  15  16  17  18  19  20  21   22  23
    fscanf(f, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu %ld %ld %ld %ld %ld %ld %llu %lu", 
        &pid2,                    // 1
        comm,                     // 2
        &state,                   // 3
        &ppid,                    // 4
        &pgrp,                    // 5
        &session,                 // 6
        &tty_nr,                  // 7
        &tpgid,                   // 8
        &flags,                   // 9
        &minflt,                  //10
        &cminflt,                 //11
        &majflt,                  //12
        &cmajflt,                 //13
        &utime,                   //14
        &stime,                   //15
        &cutime,                  //16
        &cstime,                  //17
        &priority,                //18
        &nice,                    //19
        &num_threads,             //20
        &itrealvalue,             //21
        &starttime,               //22
        &vsize                    //23
    );

    fclose(f);

    return vsize;
}

void test() {
}

void test2() {

}

void test3() {
    bson_t key;
    bson_t val;
    bson_t set_val;    
    bson_t opts;
    bson_t reply;
    bson_t result;

    bson_error_t error;
    bson_init(&key);
    bson_init(&val);
    bson_init(&set_val);

    bson_append_binary(&key, "test", -1, BSON_SUBTYPE_BINARY, (uint8_t *)"temp", 4);
    bson_append_binary(&val, "test2", -1, BSON_SUBTYPE_BINARY, (uint8_t *)"temp2", 4);
    bson_append_document(&set_val, "$set", -1, &val);

    bson_init(&opts);
    bson_append_bool(&opts, "upsert", -1, true);

    bson_init(&reply);

    mongoc_collection_update_one(MONGO_COLLECTIONS[MONGO_TEST], &key, &set_val, &opts, &reply, &error);

    mongoc_cursor_t *cursor = mongoc_collection_find(MONGO_COLLECTIONS[MONGO_TEST], MONGOC_QUERY_NONE, 0, 1, 0, key, NULL, NULL);

    bson_init(&result);
    int len = 0;
    while (mongoc_cursor_next(cursor, result)) {
        len++;
    }

    fprintf("after mongoc_cursor_next: len: %d\n", len);

    mongoc_cursor_destroy(cursor);

    bson_destroy(&result);
    bson_destroy(&reply);
    bson_destroy(&opts);
    bson_destroy(&set_val);
    bson_destroy(&val);
    bson_destroy(&key);
}

int main() {
    pid_t pid = getpid();
    unsigned long int memory_size = get_memory_size(pid);

    fprintf(stderr, "before mongoc_init: memory_size: %lu\n", memory_size);

    mongoc_init();
    MONGO_URI = mongoc_uri_new("mongodb://localhost/?wtimeoutms=10000&serverselectiontimeoutms=10000");
    MONGO_CLIENT_POOL = mongoc_client_pool_new(MONGO_URI);

    MONGO_CLIENT = mongoc_client_pool_try_pop(MONGO_CLIENT_POOL);

    MONGO_COLLECTIONS = (mongoc_collection_t**)malloc(sizeof(mongoc_collection_t *) * N_MONGO_COLLECTIONS);
    MONGO_COLLECTIONS[MONGO_MAIN] = mongoc_client_get_collection(MONGO_CLIENT, MONGO_POST_DBNAME, MONGO_MAIN_NAME);
    MONGO_COLLECTIONS[MONGO_MAIN_CONTENT] = mongoc_client_get_collection(MONGO_CLIENT, MONGO_POST_DBNAME, MONGO_MAIN_CONTENT_NAME);

    MONGO_COLLECTIONS[MONGO_TEST] = mongoc_client_get_collection(MONGO_CLIENT, MONGO_TEST_DBNAME, MONGO_TEST_NAME);

    memory_size = get_memory_size(pid);
    fprintf(stderr, "before test: memory_size: %lu\n", memory_size);

    test();

    memory_size = get_memory_size(pid);
    fprintf(stderr, "after test: memory_size: %lu\n", memory_size);

    memory_size = get_memory_size(pid);
    fprintf(stderr, "before test2: memory_size: %lu\n", memory_size);

    test2();

    memory_size = get_memory_size(pid);
    fprintf(stderr, "after test2: memory_size: %lu\n", memory_size);


    memory_size = get_memory_size(pid);
    fprintf(stderr, "before test3: memory_size: %lu\n", memory_size);

    test3();

    memory_size = get_memory_size(pid);
    fprintf(stderr, "after test3: memory_size: %lu\n", memory_size);


    memory_size = get_memory_size(pid);
    fprintf(stderr, "before test3-2: memory_size: %lu\n", memory_size);

    test3();

    memory_size = get_memory_size(pid);
    fprintf(stderr, "after test3-2: memory_size: %lu\n", memory_size);
}
