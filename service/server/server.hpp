#pragma once

#include <cstdint>
#include <goblin-engineer/core.hpp>
#include <unordered_map>

#include "api/api.hpp"
#include "api/json.hpp"
#include "components/log.hpp"
#include "components/tracy_include.hpp"
#include "service/room/room.hpp"
#include "service/server/server-routes.hpp"
#include "service/user/user.hpp"

class manager;
namespace ge = goblin_engineer;

class server : public ge::abstract_service {
public:
    using session_id = std::uint64_t;
    using user_ptr = std::shared_ptr<user>;
    using room_ptr = std::shared_ptr<room>;

    server(actor_zeta::intrusive_ptr<manager> ptr);

    void handle_request(ge::actor_address sender, session_id id, json_t& j);
    void on_db_login_response(session_id id, api::login_response& rsp);
    void on_db_register_response(session_id id, api::register_response& rsp);
    void on_db_unregister_response(session_id id, api::unregister_response& rsp);

private:
    auto gen_token_() -> std::string;
    auto gen_room_token_() -> std::string;
    void process_unlogin_(const api::unlogin_request&, session_id, ge::actor_address);
    void process_new_room_(const api::new_room_request&, session_id, ge::actor_address);
    void process_enter_room_(const api::enter_room_request&, session_id, ge::actor_address);
    void process_leave_room_(const api::leave_room_request&, session_id, ge::actor_address);
    void process_del_room_(const api::del_room_request&, session_id, ge::actor_address);
    void process_list_room_(const api::list_rooms_request&, session_id, ge::actor_address);

    template<class Rsp, class Sender, class Req, class Id>
    auto check_user(Sender sender, const Req& req, Id id,
                    const std::string& user_token, const std::string& cb) {
        Rsp rsp;
        auto it = users_.find(user_token);
        if (it == users_.end()) {
            rsp.code = Rsp::code_enum::TOKEN_INCORRECT;
            rsp.message = fmt::format("unknown token:{}", user_token);
            ge::send(sender, self(), cb, id, std::move(rsp));
            return users_.end();
        } else if (it->second->pass_hash() != req.pass_hash()()) {
            rsp.code = Rsp::code_enum::PASS_INCORRECT;
            rsp.message = "pass incorrect";
            ge::send(sender, self(), cb, id, std::move(rsp));
            return users_.end();
        }
        return it;
    }

    std::unordered_map<session_id, ge::actor_address> senders_;
    std::unordered_map<session_id, api::login_request> pending_logins_;
    std::unordered_map<session_id, api::unregister_request> pending_unregs_;
    std::unordered_map<std::string, user_ptr> users_; //TODO: clear inactive users
    std::unordered_map<std::string, room_ptr> rooms_; //TODO: clear inactive rooms
    logger log_;
};
