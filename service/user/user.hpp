#pragma once
#include "components/property.hpp"
#include <string>

class user {
public:
    property<std::string> email;
    property<std::string> pass_hash;
    property<std::string> nick;
    property<std::string> token;
};
