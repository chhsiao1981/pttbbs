#include <stdio.h>
#include <mongoc.h>
#include <sys/types.h>
#include <unistd.h>

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

void test() {
}

void test2() {

}

void test3() {
    bson_t key;
    bson_init(&key);
    bson_destroy(&key);
}

int main() {
    pid_t pid = getpid();
    int memory_size = get_memory_size(pid);

    fprintf(stderr, "before mongoc_init: memory_size: %d\n", memory_size);

    mongoc_init();
    MONGO_URI = mongoc_uri_new("mongodb://localhost/?wtimeoutms=10000&serverselectiontimeoutms=10000");
    MONGO_CLIENT_POOL = mongoc_client_pool_new(MONGO_URI);

    MONGO_CLIENT = mongoc_client_pool_try_pop(MONGO_CLIENT_POOL);

    MONGO_COLLECTIONS = malloc(sizeof(mongoc_collection_t *) * N_MONGO_COLLECTIONS);
    MONGO_COLLECTIONS[MONGO_MAIN] = mongoc_client_get_collection(MONGO_CLIENT, MONGO_POST_DBNAME, MONGO_MAIN_NAME);
    MONGO_COLLECTIONS[MONGO_MAIN_CONTENT] = mongoc_client_get_collection(MONGO_CLIENT, MONGO_POST_DBNAME, MONGO_MAIN_CONTENT_NAME);

    MONGO_COLLECTIONS[MONGO_TEST] = mongoc_client_get_collection(MONGO_CLIENT, MONGO_TEST_DBNAME, MONGO_TEST_NAME);

    memory_size = get_memory_size(pid);
    fprintf("before test: memory_size: %d\n", memory_size);

    test();

    memory_size = get_memory_size(pid);
    fprintf("after test: memory_size: %d\n", memory_size);

    memory_size = get_memory_size(pid);
    fprintf("before test2: memory_size: %d\n", memory_size);

    test2();

    memory_size = get_memory_size(pid);
    fprintf("after test2: memory_size: %d\n", memory_size);


    memory_size = get_memory_size(pid);
    fprintf("before test3: memory_size: %d\n", memory_size);

    test3();

    memory_size = get_memory_size(pid);
    fprintf("after test3: memory_size: %d\n", memory_size);
}
