Include(FetchContent)

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        master
        SYSTEM
)

FETCHCONTENT_MAKEAVAILABLE(stb)