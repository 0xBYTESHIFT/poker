#include "dispatcher.h"
#include "api/json_obj.h"
#include "manager/manager.hpp"
#include "server/server.hpp"

dispatcher::dispatcher(actor_zeta::intrusive_ptr<manager> ptr)
    : goblin_engineer::abstract_service(ptr, "dispatcher") {
    constexpr auto prefix = "dispatcher::dispatcher";
    ZoneScopedN(prefix);
    this->log_ = get_logger();
    log_.trace("{}", prefix);

    add_handler("handle_request", &dispatcher::handle_request);
    add_handler(server::cb_name_login(), &dispatcher::on_login_response);
    add_handler(server::cb_name_unlogin(), &dispatcher::on_unlogin_response);
    add_handler(server::cb_name_register(), &dispatcher::on_register_response);
    add_handler(server::cb_name_unregister(), &dispatcher::on_unregister_response);
    add_handler(server::cb_name_new_room(), &dispatcher::on_new_room_response);
    add_handler(server::cb_name_enter_room(), &dispatcher::on_enter_room_response);
    add_handler(server::cb_name_leave_room(), &dispatcher::on_leave_room_response);
    add_handler(server::cb_name_del_room(), &dispatcher::on_del_room_response);
    add_handler(server::cb_name_list_room(), &dispatcher::on_list_room_response);
}

void dispatcher::handle_request(ge::actor_address sender, session_id id, req_t& req_) {
    constexpr auto prefix = "dispatcher::handle_request";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto req = std::move(req_);
    auto& body = req->body();

    json::error_code ec;
    json j;
    j.parse(body, ec);
    if (ec.code() != json::error_code::code_t::error_none) {
        log_.error("{} error parsing json:{}", prefix, ec.mes());
        ge::send(sender, self(), cb_name_json_error, id, ec);
        return;
    }

    auto type = j.value_as<std::string>("type", ec);
    if (ec.code() != json::error_code::code_t::error_none) {
        log_.error("{} json has no field \"type\":{}", prefix, ec.mes());
        ge::send(sender, self(), cb_name_json_error, id, ec);
    }
    ge::send(addresses("server"), self(), "handle_request", self(), id, std::move(j));
}
void dispatcher::on_login_response(session_id id, api::login_response& rsp_) {
    constexpr auto prefix = "dispatcher::on_login_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto j = api::to_json(rsp);
    ge::send(addresses("rest_worker"), self(), cb_name_response, id, std::move(j));
}
void dispatcher::on_unlogin_response(session_id id, api::unlogin_response& rsp_) {
    constexpr auto prefix = "dispatcher::on_unlogin_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto j = rsp.to_json();
    ge::send(addresses("rest_worker"), self(), cb_name_response, id, std::move(j));
}
void dispatcher::on_register_response(session_id id, api::register_response& rsp_) {
    constexpr auto prefix = "dispatcher::on_register_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto j = rsp.to_json();
    ge::send(addresses("rest_worker"), self(), cb_name_response, id, std::move(j));
}
void dispatcher::on_unregister_response(session_id id, api::unregister_response& rsp_) {
    constexpr auto prefix = "dispatcher::on_unregister_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto j = rsp.to_json();
    ge::send(addresses("rest_worker"), self(), cb_name_response, id, std::move(j));
}
void dispatcher::on_new_room_response(session_id id, api::new_room_response& rsp_) {
    constexpr auto prefix = "dispatcher::on_new_room_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto j = rsp.to_json();
    ge::send(addresses("rest_worker"), self(), cb_name_response, id, std::move(j));
}
void dispatcher::on_enter_room_response(session_id id, api::enter_room_response& rsp_) {
    constexpr auto prefix = "dispatcher::on_enter_room_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto j = rsp.to_json();
    ge::send(addresses("rest_worker"), self(), cb_name_response, id, std::move(j));
}
void dispatcher::on_leave_room_response(session_id id, api::leave_room_response& rsp_) {
    constexpr auto prefix = "dispatcher::on_leave_room_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto j = rsp.to_json();
    ge::send(addresses("rest_worker"), self(), cb_name_response, id, std::move(j));
}
void dispatcher::on_del_room_response(session_id id, api::del_room_response& rsp_) {
    constexpr auto prefix = "dispatcher::on_del_room_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto j = rsp.to_json();
    ge::send(addresses("rest_worker"), self(), cb_name_response, id, std::move(j));
}
void dispatcher::on_list_room_response(session_id id, api::list_rooms_response& rsp_) {
    constexpr auto prefix = "dispatcher::on_list_room_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto j = rsp.to_json();
    ge::send(addresses("rest_worker"), self(), cb_name_response, id, std::move(j));
}