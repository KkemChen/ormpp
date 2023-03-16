# extra
option(BUILD_EXAMPLES "Build examples" ON)

# bench test
option(BUILD_BENCHMARK "Build benchmark" ON)

# unit test
option(BUILD_UNIT_TESTS "Build unit tests" ON)
if(BUILD_UNIT_TESTS)
    enable_testing()
endif()

# coverage test
option(COVERAGE_TEST "Build with unit test coverage" OFF)
if(COVERAGE_TEST)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage --coverage")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    endif()
endif()

macro(check_asan _RESULT)
    include(CheckCXXSourceRuns)
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
    check_cxx_source_runs(
            [====[
int main()
{
  return 0;
}
]====]
            ${_RESULT}
    )
    unset(CMAKE_REQUIRED_FLAGS)
endmacro()

# Enable address sanitizer
option(ENABLE_SANITIZER "Enable sanitizer(Debug+Gcc/Clang/AppleClang)" ON)
if(ENABLE_SANITIZER AND NOT MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        check_asan(HAS_ASAN)
        if(HAS_ASAN)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
        else()
            message(WARNING "sanitizer is no supported with current tool-chains")
        endif()
    else()
        message(WARNING "Sanitizer supported only for debug type")
    endif()
endif()

SET(ENABLE_SQLITE3 OFF)
SET(ENABLE_MYSQL ON)
SET(ENABLE_PG OFF)

if (ENABLE_SQLITE3)
    add_definitions(-DORMPP_ENABLE_SQLITE3)
endif()

if (ENABLE_MYSQL)
    include(cmake/mysql.cmake)
    add_definitions(-DORMPP_ENABLE_MYSQL)
endif()

if (ENABLE_PG)
    add_definitions(-DORMPP_ENABLE_PG)
endif()