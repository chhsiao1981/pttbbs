cmake_minimum_required(VERSION 2.6.4)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/original_post.1.txt ${CMAKE_CURRENT_BINARY_DIR}/original_post.1.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/original_post.2.txt ${CMAKE_CURRENT_BINARY_DIR}/original_post.2.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/original_post.3.txt ${CMAKE_CURRENT_BINARY_DIR}/original_post.3.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/original_post.4.txt ${CMAKE_CURRENT_BINARY_DIR}/original_post.4.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/original_post.5.txt ${CMAKE_CURRENT_BINARY_DIR}/original_post.5.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/original_post.6.txt ${CMAKE_CURRENT_BINARY_DIR}/original_post.6.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/original_post.7.txt ${CMAKE_CURRENT_BINARY_DIR}/original_post.7.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/original_post.8.txt ${CMAKE_CURRENT_BINARY_DIR}/original_post.8.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/original_post.9.txt ${CMAKE_CURRENT_BINARY_DIR}/original_post.9.txt COPYONLY)

file(GLOB TEST_DATA_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.txt)
message("TEST_DATA_FILES: ${TEST_DATA_FILES}")

add_custom_target(done 
    COMMAND touch done
    DEPENDS original_post.1.txt original_post.2.txt original_post.3.txt original_post.4.txt original_post.5.txt original_post.6.txt original_post.7.txt original_post.8.txt original_post.9.txt
    )
