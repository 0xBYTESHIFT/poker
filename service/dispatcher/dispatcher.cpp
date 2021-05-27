#include "api/json.hpp"
#include "dispatcher.h"
#include "manager/manager.hpp"
#include "server/server.hpp"
#include <boost/json/error.hpp>
#include <boost/json/impl/error.hpp>
#include <fmt/core.h>

dispatcher::dispatcher(actor_zeta::intrusive_ptr<manager> ptr)
    : goblin_engineer::abstract_service(ptr, "dispatcher") {
    constexpr auto prefix = "dispatcher::dispatcher";
    ZoneScopedN(prefix);
    this->log_ = get_logger();
    log_.trace("{}", prefix);

    add_handler("handle_request", &dispatcher::handle_request);
    add_handler(server::cb_name_login, &dispatcher::on_login_response);
    add_handler(server::cb_name_unlogin, &dispatcher::on_unlogin_response);
    add_handler(server::cb_name_register, &dispatcher::on_register_response);
    add_handler(server::cb_name_unregister, &dispatcher::on_unregister_response);
    add_handler(server::cb_name_new_room, &dispatcher::on_new_room_response);
    add_handler(server::cb_name_enter_room, &dispatcher::on_enter_room_response);
    add_handler(server::cb_name_leave_room, &dispatcher::on_leave_room_response);
    add_handler(server::cb_name_del_room, &dispatcher::on_del_room_response);
    add_handler(server::cb_name_list_room, &dispatcher::on_list_room_response);
    add_handler(server::cb_name_error, &dispatcher::on_server_error);
}

void dispatcher::handle_request(ge::actor_address sender, session_id id, req_t& req_) {
    constexpr auto prefix = "dispatcher::handle_request";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto req = std::move(req_);
    auto& body = req->body();

    json::error_code ec;
    json_t j = json::parse(body, ec);
    if (ec) {
        log_.error("{} error parsing json:{}", prefix, ec);
        ge::send(sender, self(), cb_name_json_error, id, ec);
        return;
    }

    std::string type;
    try {
        type = json::read<std::string>(j, "type");
    } catch (std::exception& e) {
        auto mes = fmt::format("{} error getting type: {}", prefix, e.what());
        log_.error(mes);
        ge::send(sender, self(), cb_name_json_error_str, id, mes);
        return;
    }
    /*
    if (ec.code() != json::error_code::code_t::error_none) {
        log_.error("{} json_thas no field \"type\":{}", prefix, ec.mes());
        ge::send(sender, self(), cb_name_json_error, id, ec);
    }
    */
    ge::send(addresses("server"), self(), "handle_request", self(), id, std::move(j));
}
void dispatcher::on_login_response(session_id id, api::login_response& rsp_) {
    constexpr auto prefix = "dispatcher::on_login_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto rsp = std::move(rsp_);
    auto j = rsp.to_json();
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

void dispatcher::on_server_error(session_id id, std::string& err_) {
    constexpr auto prefix = "dispatcher::on_list_room_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto err = std::move(err_);
    json_t j;
    j["message"] = std::move(err);
    ge::send(addresses("rest_worker"), self(), cb_name_response, id, std::move(j));
}
