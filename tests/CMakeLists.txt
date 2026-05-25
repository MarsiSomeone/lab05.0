
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
