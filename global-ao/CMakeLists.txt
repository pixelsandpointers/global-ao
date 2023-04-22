add_executable(global-ao-test test.cxx)
target_link_libraries(global-ao-test PRIVATE Catch2::Catch2WithMain)

add_executable(global_ao-microbenchtest microbenchtest.cxx)
target_link_libraries(global-ao-microbenchtest PRIVATE Catch2::Catch2WithMain)
