#include "db_worker.h"
#include "manager/manager.hpp"

db_worker::db_worker(actor_zeta::intrusive_ptr<manager> ptr,
                     const std::string& db_path)
    : ge::abstract_service(ptr, "db_worker")
    , db_(db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE) {
    constexpr auto prefix = "db_worker::db_worker";
    ZoneScopedN(prefix);
    this->log_ = get_logger();

    db_.exec("create table if not exists accounts (\
        nick TEXT NOT NULL UNIQUE,\
        pass TEXT NOT NULL,\
        email TEXT NOT NULL UNIQUE\
        );");
    add_handler(name_process_register(), &db_worker::process_register);
    add_handler(name_process_unregister(), &db_worker::process_unregister);
    add_handler(name_process_login(), &db_worker::process_login);
}

void db_worker::process_register(ge::actor_address sender, session_id id, api::register_request& req_) {
    constexpr auto prefix = "db_worker::process_register";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    const auto req = std::move(req_);
    auto& nick = req.nickname();
    auto& pass = req.pass_hash();
    auto& email = req.email();
    auto nicks = check_acc_exists_(email);
    api::register_response rsp;
    if (nicks.size()) {
        rsp.code = api::register_response::code_enum::NAME_TAKEN;
        rsp.message = "email is already taken";
        ge::send(sender, self(), cb_name_register(), id, std::move(rsp));
        return;
    }
    try {
        auto q_ins = fmt::format("insert into accounts(nick, pass, email)\
                                values('{}', '{}', '{}');",
                                 nick(), pass(), email());
        db_.exec(q_ins);
    } catch (SQLite::Exception& e) {
        log_.error("{} sqlite error:{}", prefix, e.what());
        return;
    }
    rsp.code = api::register_response::code_enum::OK;
    rsp.message = "success";
    ge::send(sender, self(), cb_name_register(), id, std::move(rsp));
}

void db_worker::process_unregister(ge::actor_address sender, session_id id, api::unregister_request& req_) {
    constexpr auto prefix = "db_worker::process_unregister";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    const auto req = std::move(req_);
    auto& pass = req.pass_hash();
    auto& email = req.email();
    auto nicks = check_acc_exists_(email);
    api::unregister_response rsp;
    if (nicks.empty()) {
        rsp.code = api::unregister_response::code_enum::NO_NAME;
        rsp.message = "email is unknown";
        ge::send(sender, self(), cb_name_unregister(), id, std::move(rsp));
        return;
    }
    int count = 0;
    try {
        auto q_ins = fmt::format("delete from accounts where email = '{}' and pass = '{}';",
                                 email(), pass());
        count = db_.exec(q_ins);
    } catch (SQLite::Exception& e) {
        log_.error("{} sqlite error:{}", prefix, e.what());
        return;
    }
    if (count == 0) {
        rsp.code = api::unregister_response::code_enum::NO_PASS;
        rsp.message = "password is incorrect";
    } else {
        rsp.code = api::unregister_response::code_enum::OK;
        rsp.message = "success";
    }
    ge::send(sender, self(), cb_name_unregister(), id, std::move(rsp));
}

void db_worker::process_login(ge::actor_address sender, session_id id, api::login_request& req_) {
    constexpr auto prefix = "db_worker::process_login";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto req = std::move(req_);
    auto& pass = req.pass_hash();
    auto& email = req.email();
    auto nicks = check_acc_exists_(email);
    api::login_response rsp;
    if (nicks.empty()) {
        rsp.code = api::login_response::code_enum::NAME_UNKNOWN;
        rsp.message = fmt::format("email {} is unknown", email());
        ge::send(sender, self(), cb_name_login(), id, std::move(rsp));
        return;
    }
    nicks = check_acc_exists_(email, pass);
    if (nicks.empty()) {
        rsp.code = api::login_response::code_enum::PASS_INCORRECT;
        rsp.message = "pass is incorrect";
    } else {
        rsp.code = api::login_response::code_enum::OK;
        rsp.message = fmt::format("success");
    }
    ge::send(sender, self(), cb_name_login(), id, std::move(rsp));
}

auto db_worker::check_acc_exists_(const std::string& email, const std::optional<std::string>& pass) -> std::vector<std::string> {
    constexpr auto prefix = "db_worker::check_acc_exists_";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    std::vector<std::string> nicks;
    std::string q_sel = fmt::format("select * from accounts where email = '{}'",
                                    email);
    if (pass.has_value()) {
        q_sel += fmt::format(" and pass = '{}'", pass.value());
    }
    q_sel += ";";
    SQLite::Statement query_sel(db_, q_sel);
    while (query_sel.executeStep()) {
        const char* n = query_sel.getColumn("nick");
        nicks.emplace_back(n);
    }
    return nicks;
}
