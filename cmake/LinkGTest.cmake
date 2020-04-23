include(FetchContent)

macro(LinkGTest TARGET ACCESS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.10.0
    )

    FetchContent_GetProperties(googletest)

    if (NOT googletest_POPULATED)
        FetchContent_Populate(googletest)

        # This excludes googletest from being rebuilt and ALL_BUILD is built
        # it will only be built when a target is built that has a dependency
        # on google test/mock
        add_subdirectory(
            ${googletest_SOURCE_DIR}
            ${googletest_BINARY_DIR}
            EXCLUDE_FROM_ALL)

        # Set the target's folders
        set_target_properties(gmock PROPERTIES FOLDER ${PROJECT_NAME}/thirdparty)
        set_target_properties(gmock_main PROPERTIES FOLDER ${PROJECT_NAME}/thirdparty)
        set_target_properties(gtest PROPERTIES FOLDER ${PROJECT_NAME}/thirdparty)
        set_target_properties(gtest_main PROPERTIES FOLDER ${PROJECT_NAME}/thirdparty)
    endif()
    
    # This prevents overriding the parent's projects compiler and linker settings
    if (WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    target_link_libraries(${TARGET} ${ACCESS} gtest_main)

    add_dependencies(${TARGET} gtest_main)
endmacro()