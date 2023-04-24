Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        3f0283d  # v3.3.2
        SYSTEM
        FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(Catch2)
