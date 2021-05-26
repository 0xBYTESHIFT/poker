#pragma once

#include <SQLiteCpp/SQLiteCpp.h>
#include <cstdint>
#include <goblin-engineer/core.hpp>
#include <optional>
#include <string>
#include <vector>

#include "api/api.hpp"
#include "components/log.hpp"
#include "components/tracy_include.hpp"

class manager;
namespace ge = goblin_engineer;

class db_worker : public ge::abstract_service {
public:
    using session_id = std::uint64_t;

    db_worker(actor_zeta::intrusive_ptr<manager> ptr,
              const std::string& db_path);

    void process_register(ge::actor_address sender, session_id id, api::register_request& req);
    void process_unregister(ge::actor_address sender, session_id id, api::unregister_request& req);
    void process_login(ge::actor_address sender, session_id id, api::login_request& req);

    static auto name_process_login() { return "process_login"; }
    static auto name_process_register() { return "process_register"; }
    static auto name_process_unregister() { return "process_unregister"; }
    static auto cb_name_login() { return "on_login_response"; }
    static auto cb_name_register() { return "on_register_response"; }
    static auto cb_name_unregister() { return "on_unregister_response"; }

private:
    auto check_acc_exists_(const std::string& email,
                           const std::optional<std::string>& pass = std::nullopt) -> std::vector<std::string>;
    logger log_;
    SQLite::Database db_;
};
