#include "api/api.hpp"
#include "db_worker/db_worker.h"
#include "manager/manager.hpp"
#include "server.hpp"
#include <exception>
#include <random>

server::server(actor_zeta::intrusive_ptr<manager> ptr)
    : ge::abstract_service(ptr, server_routes::name) {
    constexpr auto prefix = "server::server";
    ZoneScopedN(prefix);
    this->log_ = get_logger();
    add_handler(server_routes::name_handle_request, &server::handle_request);
    add_handler(db_worker_routes::cb_name_login, &server::on_db_login_response);
    add_handler(db_worker_routes::cb_name_register, &server::on_db_register_response);
    add_handler(db_worker_routes::cb_name_unregister, &server::on_db_unregister_response);
}

auto server::handle_request(ge::actor_address sender, session_id id, json_t& j_) -> void {
    constexpr auto prefix = "server::handle_request";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto j = std::move(j_);

    json::error_code ec;
    try {
        auto type = json::read<std::string>(j, "type");
        auto db_addr = addresses(db_worker_routes::name);
        if (type == api::register_request::type()()) {
            auto req = api::register_request(j);
            senders_[id] = sender;
            ge::send(db_addr, self(), db_worker_routes::name_process_register, self(), id, std::move(req));
        } else if (type == api::unregister_request::type()()) {
            auto req = api::unregister_request(j);
            senders_[id] = sender;
            pending_unregs_[id] = req;
            ge::send(db_addr, self(), db_worker_routes::name_process_unregister, self(), id, std::move(req));
        } else if (type == api::login_request::type()()) {
            auto req = api::login_request(j);
            senders_[id] = sender;
            pending_logins_[id] = req;
            ge::send(db_addr, self(), db_worker_routes::name_process_login, self(), id, std::move(req));
        } else if (type == api::unlogin_request::type()()) {
            auto req = api::unlogin_request(j);
            process_unlogin_(req, id, sender);
        } else if (type == api::new_room_request::type().downcast()) {
            auto req = api::new_room_request(j);
            process_new_room_(req, id, sender);
        } else if (type == api::enter_room_request::type().downcast()) {
            auto req = api::enter_room_request(j);
            process_enter_room_(req, id, sender);
        } else if (type == api::leave_room_request::type().downcast()) {
            auto req = api::leave_room_request(j);
            process_leave_room_(req, id, sender);
        } else if (type == api::del_room_request::type().downcast()) {
            auto req = api::del_room_request(j);
            process_del_room_(req, id, sender);
        } else if (type == api::list_rooms_request::type().downcast()) {
            auto req = api::list_rooms_request(j);
            process_list_room_(req, id, sender);
        } else {
            //send error
        }
    } catch (std::exception& e) {
        auto mes = fmt::format("{} error: {}", prefix, e.what());
        log_.error(mes);
        ge::send(sender, self(), server_routes::cb_name_error, id, std::move(mes));
    }
}

void server::on_db_login_response(session_id id, api::login_response& rsp_) {
    constexpr auto prefix = "server::on_db_login_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto it = senders_.find(id);
    if (it == senders_.end()) {
        log_.error("{} unknown id:{}", prefix, id);
        return;
    }
    auto& sender = it->second;
    if (rsp.code()() == api::login_response::code_enum::OK) {
        auto token = gen_token_();
        rsp.token = token;
        auto u = std::make_shared<user>();
        u->token() = token;
        u->email() = pending_logins_.at(id).email();
        u->pass_hash() = pending_logins_.at(id).pass_hash();
        users_[token] = std::move(u);
    }
    ge::send(sender, self(), server_routes::cb_name_login, id, std::move(rsp));
    senders_.erase(id);
    pending_logins_.erase(id);
}

void server::on_db_register_response(session_id id, api::register_response& rsp_) {
    constexpr auto prefix = "server::on_db_register_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto it = senders_.find(id);
    if (it == senders_.end()) {
        log_.error("{} unknown id:{}", prefix, id);
        return;
    }
    auto& sender = it->second;
    ge::send(sender, self(), server_routes::cb_name_register, id, std::move(rsp));
    senders_.erase(id);
}

void server::on_db_unregister_response(session_id id, api::unregister_response& rsp_) {
    constexpr auto prefix = "server::on_db_unregister_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    const auto rsp = std::move(rsp_);
    auto it = senders_.find(id);
    if (it == senders_.end()) {
        log_.error("{} unknown id:{}", prefix, id);
        return;
    }
    auto& sender = it->second;
    if (rsp.code()() == api::unregister_response::code_enum::OK) {
        auto unreg = pending_unregs_.at(id);
        auto& user_token = unreg.token()();
        for (auto& [room_token, room_ptr] : rooms_) {
            auto& users = room_ptr->users();
            auto room_user_it = users.find(user_token);
            if (room_user_it == users.end()) {
                continue;
            }
            users.erase(room_user_it);
            if (users.empty()) {
                rooms_.erase(room_token);
            }
            break;
        }
        users_.erase(user_token);
    }
    ge::send(sender, self(), server_routes::cb_name_unregister, id, std::move(rsp));
    pending_unregs_.erase(id);
    senders_.erase(id);
}

auto server::gen_token_() -> std::string {
    constexpr auto prefix = "server::gen_token_";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    const static std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const static int size = 16;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(0, alphabet.size() - 1);
    std::string result;
    result.resize(size, '\0');
    do {
        for (size_t i = 0; i < size; i++) {
            result[i] = alphabet[dist(gen)];
        }
    } while (users_.find(result) != users_.end());
    return result;
}

auto server::gen_room_token_() -> std::string {
    constexpr auto prefix = "server::gen_room_token_";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    const static std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const static int size = 16;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(0, alphabet.size() - 1);
    std::string result;
    result.resize(size, '\0');
    do {
        for (size_t i = 0; i < size; i++) {
            result[i] = alphabet[dist(gen)];
        }
    } while (rooms_.find(result) != rooms_.end());
    return result;
}

void server::process_unlogin_(const api::unlogin_request& req, session_id id, ge::actor_address sender) {
    constexpr auto prefix = "server::process_unlogin_";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto& user_token = req.token()();
    auto it = check_user<api::unlogin_response>(sender, req, id, user_token, server_routes::cb_name_unlogin);
    if (it == users_.end()) {
        return;
    }
    api::unlogin_response rsp;
    rsp.code = api::unlogin_response::code_enum::OK;
    rsp.message = "unlogin successful";
    for (auto& [room_token, room_ptr] : rooms_) {
        auto& users = room_ptr->users();
        auto room_user_it = users.find(user_token);
        if (room_user_it == users.end()) {
            continue;
        }
        users.erase(room_user_it);
        if (users.empty()) {
            rooms_.erase(room_token);
        }
        break;
    }
    users_.erase(req.token()());
    ge::send(sender, self(), server_routes::cb_name_unlogin, id, std::move(rsp));
}
