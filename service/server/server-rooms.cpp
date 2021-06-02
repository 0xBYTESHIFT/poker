#include "server.hpp"

void server::process_new_room_(const api::new_room_request& req, session_id id, ge::actor_address sender) {
    //TODO: send errors
    ZoneScopedTraceL("server::process_new_room_");
    auto& user_token = req.token().downcast();
    auto it = check_user<api::new_room_response>(sender, req, id, user_token, server_routes::cb_name_new_room);
    if (it == users_.end()) {
        return;
    }
    auto& user_ptr = it->second;
    auto r = std::make_shared<room>();
    r->token() = gen_room_token_();
    r->pass_hash() = req.pass_hash();
    r->hidden() = req.room_hidden();
    rooms_[r->token()] = r;

    api::new_room_response rsp;
    rsp.code() = api::new_room_response::code_enum::OK;
    rsp.message() = "ok";
    rsp.room_token() = r->token();
    ge::send(sender, self(), server_routes::cb_name_new_room, id, std::move(rsp));
}

void server::process_enter_room_(const api::enter_room_request& req, session_id id, ge::actor_address sender) {
    //TODO: send errors
    ZoneScopedTraceL("server::process_enter_room_");
    auto user_token = req.token().downcast();
    auto it = check_user<api::enter_room_response>(sender, req, id, user_token, server_routes::cb_name_enter_room);
    if (it == users_.end()) {
        return;
    }
    api::enter_room_response rsp;
    auto& user_ptr = it->second;
    auto room_it = rooms_.find(req.room_token().downcast());
    if (room_it == rooms_.end()) {
        rsp.code() = api::enter_room_response::code_enum::ROOM_TOKEN_INCORRECT;
        rsp.message() = fmt::format("unknown room token:{}", req.room_token().downcast());
    } else if (req.pass_hash()() != room_it->second->pass_hash()) {
        rsp.code() = api::enter_room_response::code_enum::ROOM_PASS_INCORRECT;
        rsp.message() = "incorrect room pass";
    } else {
        rsp.code() = api::enter_room_response::code_enum::OK;
        rsp.message() = "ok";
        room_it->second->users().emplace(user_token, user_ptr);
    }
    ge::send(sender, self(), server_routes::cb_name_enter_room, id, std::move(rsp));
}

void server::process_leave_room_(const api::leave_room_request& req, session_id id, ge::actor_address sender) {
    ZoneScopedTraceL("server::process_leave_room_");
    auto& room_token = req.room_token().downcast();
    auto& user_token = req.token().downcast();
    auto it = check_user<api::leave_room_response>(sender, req, id, user_token, server_routes::cb_name_leave_room);
    if (it == users_.end()) {
        return;
    }
    api::leave_room_response rsp;
    auto room_it = rooms_.find(room_token);
    if (room_it == rooms_.end()) {
        rsp.code() = api::leave_room_response::code_enum::ROOM_TOKEN_INCORRECT;
        rsp.message() = fmt::format("unknown room token:{}", room_token);
    } else {
        auto& users = room_it->second->users();
        auto room_user_it = users.find(user_token);
        if (room_user_it == users.end()) {
            rsp.code() = api::leave_room_response::code_enum::USER_NOT_IN_THE_ROOM;
            rsp.message() = "user not in the room";
        } else {
            rsp.code() = api::leave_room_response::code_enum::OK;
            rsp.message() = "ok";
            log_.info("{} user {} left room {}", prefix, user_token, room_token);
            users.erase(user_token);
            if (users.empty()) {
                log_.info("{} erasing empty room {}", prefix, room_token);
                rooms_.erase(room_token);
            }
        }
    }
    ge::send(sender, self(), server_routes::cb_name_leave_room, id, std::move(rsp));
}
void server::process_del_room_(const api::del_room_request& req, session_id id, ge::actor_address sender) {
    ZoneScopedTraceL("server::process_del_room_");
    api::del_room_response rsp;
    using code_enum = decltype(rsp)::code_enum;
    auto user_token = req.token().downcast();
    auto room_token = req.room_token().downcast();
    auto it = check_user<api::del_room_response>(sender, req, id, user_token, server_routes::cb_name_del_room);
    if (it == users_.end()) {
        return;
    }
    auto& user_ptr = it->second;
    auto room_it = rooms_.find(room_token);
    if (room_it == rooms_.end()) {
        rsp.code() = code_enum::ROOM_TOKEN_INCORRECT;
        rsp.message() = fmt::format("unknown room token:{}", room_token);
    } else {
        auto& room_ptr = room_it->second;
        if (!room_ptr->has_rights_to_delete(user_ptr)) {
            rsp.code() = code_enum::NO_RIGHTS;
            rsp.message() = "insufficient rights";
        } else if (room_ptr->users().size()) {
            rsp.code() = code_enum::ROOM_NOT_EMPTY;
            rsp.message() = "room is not empty";
        } else {
            rsp.code() = code_enum::OK;
            rsp.message() = "ok";
            rooms_.erase(room_token);
        }
    }
    ge::send(sender, self(), server_routes::cb_name_del_room, id, std::move(rsp));
}

void server::process_list_room_(const api::list_rooms_request& req, session_id id, ge::actor_address sender) {
    ZoneScopedTraceL("server::process_list_room_");
    auto user_token = req.token().downcast();
    auto it = check_user<api::list_rooms_response>(sender, req, id, user_token, server_routes::cb_name_list_room);
    if (it == users_.end()) {
        return;
    }
    auto& user_ptr = it->second;
    api::list_rooms_response rsp;
    rsp.code() = api::list_rooms_response::code_enum::OK;
    rsp.message() = "ok";
    auto& rooms_data = rsp.rooms_data().downcast();
    rooms_data.reserve(rooms_.size());
    for (auto& [r_token, r_ptr] : rooms_) {
        if (r_ptr->hidden()) {
            continue;
        }
        decltype(rsp)::room_data_t data;
        data.name = "unnamed"; //FIXME:
        data.token = r_token;
        data.count = r_ptr->users().size();
        rooms_data.emplace_back(data);
    }
    ge::send(sender, self(), server_routes::cb_name_list_room, id, std::move(rsp));
}
