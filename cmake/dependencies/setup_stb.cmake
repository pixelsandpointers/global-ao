Include(FetchContent)

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG 5736b15
        SYSTEM
)

FETCHCONTENT_MAKEAVAILABLE(stb)

# manually add the library since it doesn't have a CMakeLists.txt

# first we get all the source files. I believe it's fine to use glob in this case since the source aren't edited by us
file(GLOB_RECURSE stb_SOURCE_FILES RELATIVE ${stb_SOURCE_DIR} ${stb_SOURCE_DIR}/*.c)

# we manually remove test files
foreach (f in ${stb_SOURCE_FILES})
    if (${f} MATCHES ".*tests?.+")
        list(REMOVE_ITEM stb_SOURCE_FILES ${f})
    endif ()
endforeach ()

add_library(stb ${stb_SOURCE_DIR}/stb_image.h)
target_include_directories(stb PUBLIC ${stb_SOURCE_DIR})
set_target_properties(stb PROPERTIES LINKER_LANGUAGE CXX)