cmake_minimum_required(VERSION 2.6.4)

file(GLOB TEST_DATA_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/" "*.txt")
message("TEST_DATA_FILES: ${TEST_DATA_FILES}")

foreach( test_data_file ${TEST_DATA_FILES})
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${test_data_file}
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/${test_data_file} ${CMAKE_CURRENT_BINARY_DIR}/${test_data_file}
        )
endforeach(test_data_file)

add_custom_target(all_test_data
    ALL DEPENDS ${TEST_DATA_FILES}
    )
