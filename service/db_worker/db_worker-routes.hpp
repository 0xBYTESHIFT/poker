#pragma once

namespace db_worker_routes {
    static const auto inline name = "db_worker";
    static const auto inline name_process_login = "process_login";
    static const auto inline name_process_register = "process_register";
    static const auto inline name_process_unregister = "process_unregister";
    static const auto inline cb_name_login = "on_login_response";
    static const auto inline cb_name_register = "on_register_response";
    static const auto inline cb_name_unregister = "on_unregister_response";
}; // namespace db_worker_routes
