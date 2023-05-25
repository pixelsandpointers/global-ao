enable_testing()

include(Catch)

function(global_ao_add_test NAME SOURCE)
    # Define the supported set of keywords
    set(prefix GLOBAL_AO_ADD_TEST)
    set(noValues "")
    set(singleValues "")
    set(multiValues LINK_LIBRARIES)

    # Process the arguments passed in
    include(CMakeParseArguments)
    cmake_parse_arguments(
            ${prefix}
            "${noValues}" "${singleValues}" "${multiValues}"
            ${ARGN}
    )

    # Add the test
    set(TARGET_NAME "test-${CMAKE_PROJECT_NAME}-${NAME}")

    add_executable(${TARGET_NAME}
            ${SOURCE}
            )

    target_link_libraries(${TARGET_NAME}
            PRIVATE
            Catch2::Catch2WithMain
            ${${prefix}_LINK_LIBRARIES}
            )

    catch_discover_tests(${TARGET_NAME} TEST_SPEC "*")  # Run all test tags regardless of tags (including benchmarks)
endfunction()
