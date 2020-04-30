# Utility Public Interface

set(sources 
    FileUtil.cpp)

set(includes 
    FileUtil.h
    ScopedTimer.h)

add_library(utility STATIC ${sources} ${includes})

target_include_directories(reshaper
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(utility PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Treat warnings as errors
if(MSVC)
    target_compile_options(utility PRIVATE /W4 /WX)
else()
    target_compile_options(utility PRIVATE -Wall -Wextra -pedantic -Werror)
endif()