include(FetchContent)

macro(LinkSTB TARGET ACCESS)
    FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb
        GIT_TAG f54acd4e13430c5122cab4ca657705c84aa61b08
    )

    FetchContent_GetProperties(stb)

    if (NOT stb_POPULATED)
        FetchContent_Populate(stb)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${stb_SOURCE_DIR})
endmacro()