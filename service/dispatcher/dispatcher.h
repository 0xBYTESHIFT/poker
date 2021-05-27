#pragma once

#include <goblin-engineer/core.hpp>
#include <restinio/all.hpp>

#include "api/api.hpp"
#include "components/log.hpp"
#include "components/tracy_include.hpp"
#include "dispatcher-routes.hpp"

class manager;
namespace ge = goblin_engineer;

class dispatcher : public ge::abstract_service {
public:
    enum class code_t {
        OK,
        no_type,
        unknown_type
    };
    class error_code {
        friend class dispatcher;
        code_t e_;
        std::string mes_;

    public:
        auto code() const -> code_t { return e_; }
        auto mes() const -> const std::string& { return mes_; }
    };

    using req_t = std::shared_ptr<restinio::generic_request_t<restinio::no_extra_data_factory_t::data_t>>;
    using session_id = std::uint64_t;

    explicit dispatcher(actor_zeta::intrusive_ptr<manager> ptr);

    void handle_request(ge::actor_address sender, session_id id, req_t& req);

    void on_login_response(session_id, api::login_response& rsp);
    void on_unlogin_response(session_id, api::unlogin_response& rsp);
    void on_register_response(session_id, api::register_response& rsp);
    void on_unregister_response(session_id, api::unregister_response& rsp);
    void on_new_room_response(session_id, api::new_room_response& rsp);
    void on_enter_room_response(session_id, api::enter_room_response& rsp);
    void on_leave_room_response(session_id, api::leave_room_response& rsp);
    void on_list_room_response(session_id, api::list_rooms_response& rsp);
    void on_del_room_response(session_id, api::del_room_response& rsp);
    void on_server_error(session_id, std::string& err);

private:
    logger log_;
};
