#pragma once
#include "api/val_t.hpp"
#include "components/property.hpp"
#include "components/tracy_include.hpp"
#include "json_obj.h"
#include "rapidjson/prettywriter.h"
#include <codecvt>
#include <locale>
#include <string>

#include "rooms/api-del_room.hpp"
#include "rooms/api-enter_room.hpp"
#include "rooms/api-leave_room.hpp"
#include "rooms/api-list_rooms.hpp"
#include "rooms/api-new_room.hpp"

#include "basic/api-connect.hpp"
#include "basic/api-login.hpp"
#include "basic/api-register.hpp"
#include "basic/api-unlogin.hpp"
#include "basic/api-unreg.hpp"

namespace api {
}; // namespace api
