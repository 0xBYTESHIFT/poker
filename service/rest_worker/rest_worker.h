#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <goblin-engineer/core.hpp>
#include <map>
#include <memory>
#include <restinio/all.hpp>
#include <string>

#include "api/json.hpp"
#include "components/log.hpp"
#include "components/tracy_include.hpp"
#include "dispatcher/dispatcher-routes.hpp"
#include "dispatcher/dispatcher.h"
#include "rest_worker-routes.hpp"

class manager;
namespace ge = goblin_engineer;
namespace rst = restinio;

class rest_worker : public ge::abstract_service {
public:
    using req_t = std::shared_ptr<rst::generic_request_t<rst::no_extra_data_factory_t::data_t>>;
    using session_id = std::uint64_t;

    rest_worker(actor_zeta::intrusive_ptr<manager> ptr, boost::asio::io_context& ctx, size_t port);

    template<typename RESP>
    static auto init_resp_(RESP&& resp) -> RESP;

    auto handle_request(const req_t& req) -> rst::request_handling_status_t;
    void on_dispatcher_json_error(session_id id, json::error_code ec);
    void on_dispatcher_json_error_str(session_id id, std::string& err);
    void on_dispatcher_error(session_id id, dispatcher::error_code ec);
    void on_dispatcher_response(session_id id, json_t& j);

private:
    using router_t = rst::router::express_router_t<rst::router::std_regex_engine_t>;
    using traits_t = rst::traits_t<rst::asio_timer_manager_t, rst::null_logger_t, router_t>;
    void run_();
    auto create_server_handler_() -> std::unique_ptr<router_t>;

    rst::running_server_handle_t<traits_t> server_;
    std::unordered_map<session_id, req_t> requests;
    session_id last_id_ = 0;
    logger log_;
    boost::asio::io_context& ctx_;
    size_t port_;
};

template<typename RESP>
inline auto rest_worker::init_resp_(RESP&& resp) -> RESP {
    resp.append_header("Server", "RESTinio sample server /v.0.4");
    resp.append_header_date_field()
        .append_header("Content-Type", "text/plain; charset=utf-8");

    return std::forward<RESP>(resp);
}
