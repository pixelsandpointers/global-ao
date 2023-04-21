add_executable(test test.cxx)
target_link_libraries(test PRIVATE Catch2::Catch2WithMain)

add_executable(microbenchtest microbenchtest.cxx)
target_link_libraries(microbenchtest PRIVATE Catch2::Catch2WithMain)
