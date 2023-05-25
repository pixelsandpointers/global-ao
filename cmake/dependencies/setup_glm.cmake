include(FetchContent)

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        bf71a83  # v0.9.9.8
        SYSTEM
)

FetchContent_MakeAvailable(glm)
