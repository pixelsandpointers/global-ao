Include(FetchContent)

FetchContent_Declare(
        TinyOBJLoader
        GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader
        GIT_TAG 853f059  # v2
        SYSTEM
)

FetchContent_MakeAvailable(TinyOBJLoader)