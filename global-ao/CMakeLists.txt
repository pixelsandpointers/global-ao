add_executable(${CMAKE_PROJECT_NAME}-test test.cxx)
target_link_libraries(${CMAKE_PROJECT_NAME}-test PRIVATE Catch2::Catch2WithMain)

add_executable(${CMAKE_PROJECT_NAME}-microbenchtest microbenchtest.test.cxx)
target_link_libraries(${CMAKE_PROJECT_NAME}-microbenchtest PRIVATE Catch2::Catch2WithMain)
