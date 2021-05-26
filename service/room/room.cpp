#include "room.hpp"
#include "service/user/user.hpp"

auto room::has_rights_to_delete(const user_ptr& u) -> bool {
    return u == this->admin();
}