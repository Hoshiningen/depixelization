# Reshaper implementation

include(${CMAKE_DIR}/LinkSTB.cmake)

set(PRIVATE_INCLUDE_DIR ${SOURCE_DIR}/reshaper/include/private)

set(sources ImageView.cpp)
set(includes ${PRIVATE_INCLUDE_DIR}/ImageView.h)

add_library(reshaper-impl STATIC ${sources} ${includes})

# Link things
LinkSTB(reshaper-impl PRIVATE)

target_include_directories(reshaper-impl PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${PRIVATE_INCLUDE_DIR})

target_include_directories(reshaper-impl PRIVATE
    ${PRIVATE_INCLUDE_DIR}/../reshaper)

set_target_properties(reshaper-impl PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO)

# Treat warnings as errors
if(MSVC)
    target_compile_options(reshaper-impl PRIVATE /W4 /WX)
else()
    target_compile_options(reshaper-impl PRIVATE -Wall -Wextra -pedantic -Werror)
endif()
