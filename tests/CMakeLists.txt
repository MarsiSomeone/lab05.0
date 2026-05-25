
add_executable(tests
    AccountTests.cpp
    TransactionTest.cpp
)


target_link_libraries(tests
    banking
    gtest_main
    gmock
    pthread
)


target_include_directories(tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/mocks
    ${CMAKE_SOURCE_DIR}
)


include(GoogleTest)
gtest_discover_tests(tests)

if(COVERAGE)
    target_compile_options(tests PRIVATE -O0 -g --coverage)
    target_link_options(tests PRIVATE --coverage)
endif()
