include(FetchContent)

macro(LinkGTest TARGET ACCESS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG master
    )

    FetchContent_GetProperties(googletest)

    if (NOT googletest_POPULATED)
        FetchContent_Populate(googletest)

        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

        # Set the target's folders
        set_target_properties(gmock PROPERTIES FOLDER Depixelization/thirdparty)
        set_target_properties(gmock_main PROPERTIES FOLDER Depixelization/thirdparty)
        set_target_properties(gtest PROPERTIES FOLDER Depixelization/thirdparty)
        set_target_properties(gtest_main PROPERTIES FOLDER Depixelization/thirdparty)
    endif()
        
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    target_link_libraries(${TARGET} ${ACCESS} gtest_main)
    target_include_directories(${TARGET} ${ACCESS} ${googletest_SOURCE_DIR}/include)

    add_dependencies(${TARGET} gtest_main)
endmacro()