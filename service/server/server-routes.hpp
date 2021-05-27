#pragma once

namespace server_routes {
    const static inline auto name = "server";
    const static inline auto name_handle_request = "handle_request";
    const static inline auto cb_name_login = "on_login_response";
    const static inline auto cb_name_unlogin = "on_unlogin_response";
    const static inline auto cb_name_register = "on_register_response";
    const static inline auto cb_name_unregister = "on_unregister_response";
    const static inline auto cb_name_new_room = "on_new_room_response";
    const static inline auto cb_name_enter_room = "on_enter_room_response";
    const static inline auto cb_name_leave_room = "on_leave_room_response";
    const static inline auto cb_name_del_room = "on_del_room_response";
    const static inline auto cb_name_list_room = "on_list_room_response";
    const static inline auto cb_name_error = "on_server_error";
}; // namespace server_routes
