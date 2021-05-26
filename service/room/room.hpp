#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include "components/property.hpp"

class user;
using user_ptr = std::shared_ptr<user>;
class room {
public:
    property<std::string> token;
    property<std::unordered_map<std::string, user_ptr>> users;
    property<user_ptr> admin;

    auto has_rights_to_delete(const user_ptr& u) -> bool;
};