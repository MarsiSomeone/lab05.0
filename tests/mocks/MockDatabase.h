#pragma once
#include <gmock/gmock.h>

class MockDatabase {
public:
    MOCK_METHOD(void, Save, (const std::string& data), ());
    MOCK_METHOD(bool, Connect, (), ());
    MOCK_METHOD(void, Disconnect, (), ());
};
