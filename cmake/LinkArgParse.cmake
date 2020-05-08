include(FetchContent)

macro(LinkArgParse TARGET ACCESS)
    FetchContent_Declare(
        argparse
        GIT_REPOSITORY https://github.com/p-ranav/argparse
        GIT_TAG v2.0
    )

    FetchContent_GetProperties(argparse)

    if (NOT argparse_POPULATED)
        FetchContent_Populate(argparse)
    endif()

    target_include_directories(${TARGET} ${ACCESS} ${argparse_SOURCE_DIR}/include)
endmacro()