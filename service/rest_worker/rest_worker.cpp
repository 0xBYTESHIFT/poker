#include "rest_worker.h"
#include "manager/manager.hpp"
#include <mutex>

static std::mutex mt;
auto rest_worker::create_server_handler_() -> std::unique_ptr<router_t> {
    constexpr auto prefix = "rest_worker::create_server_handler_";
    ZoneScopedN(prefix);
    auto router = std::make_unique<router_t>();

    router->http_get(
        "/",
        [](auto req, auto) {
            rest_worker::init_resp_(req->create_response())
                .append_header(restinio::http_field::content_type, "text/plain; charset=utf-8")
                .set_body("Hello world!")
                .done();

            return restinio::request_accepted();
        });

    router->http_get(
        "/json",
        [this](auto req, auto) {
            return this->handle_request(req);
        });

    return router;
}

rest_worker::rest_worker(actor_zeta::intrusive_ptr<manager> ptr, boost::asio::io_context& ctx, size_t port)
    : ge::abstract_service(ptr, "rest_worker")
    , ctx_(ctx)
    , port_(port) {
    constexpr auto prefix = "rest_worker::rest_worker";
    ZoneScopedN(prefix);
    this->log_ = get_logger();
    log_.trace("{}", prefix);

    add_handler("run", &rest_worker::run_);
    add_handler(dispatcher::cb_name_json_error, &rest_worker::on_dispatcher_json_error);
    add_handler(dispatcher::cb_name_error, &rest_worker::on_dispatcher_error);
    add_handler(dispatcher::cb_name_response, &rest_worker::on_dispatcher_response);
}

void rest_worker::run_() {
    constexpr auto prefix = "rest_worker::run_";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    using namespace std::chrono;

    this->server_ = restinio::run_async(
        restinio::own_io_context(),
        restinio::server_settings_t<traits_t>()
            .address("localhost")
            .port(port_)
            .request_handler(create_server_handler_())
            .read_next_http_message_timelimit(5ms)
            .write_http_response_timelimit(5ms),
        4);
    log_.debug("{} run ended", prefix);
    //.handle_request_timeout(1s));
}

auto rest_worker::handle_request(const req_t& req) -> restinio::request_handling_status_t {
    constexpr auto prefix = "rest_worker::handle_request";
    ZoneScopedN(prefix);
    std::lock_guard<std::mutex> lock(mt);
    log_.trace("{}", prefix);
    if (!req) {
        log_.error("{} no request!", prefix);
        return restinio::request_rejected();
    }
    log_.debug("{} con:{} req:{} body:{}", prefix, req->connection_id(), req->request_id(), req->body());
    requests.emplace(last_id_, req);

    ge::send(addresses("dispatcher"), self(), "handle_request", self(), last_id_, req);
    last_id_++;

    return restinio::request_accepted();
}

void rest_worker::on_dispatcher_json_error(session_id id, json::error_code ec) {
    constexpr auto prefix = "rest_worker::on_dispatcher_json_error";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto it = requests.find(id);
    if (it == requests.end()) {
        log_.error("{} unknown id:{}", prefix, id);
        return;
    }
    auto& req = it->second;
    auto mes = ec.mes();
    log_.error("{} id:{} mes:{}", prefix, id, mes);
    rest_worker::init_resp_(req->create_response())
        .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
        .set_body(fmt::format("{{\"message\" : \"{}\"}}", mes))
        .done();
    requests.erase(id);
}

void rest_worker::on_dispatcher_error(session_id id, dispatcher::error_code ec) {
    constexpr auto prefix = "rest_worker::on_dispatcher_error";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto it = requests.find(id);
    if (it == requests.end()) {
        log_.error("{} unknown id:{}", prefix, id);
        return;
    }
    auto& req = it->second;
    auto mes = ec.mes();
    log_.error("{} id:{} mes:{}", prefix, id, mes);
    rest_worker::init_resp_(req->create_response())
        .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
        .set_body(fmt::format("{{\"message\" : \"{}\"}}", mes))
        .done();
    requests.erase(id);
}
void rest_worker::on_dispatcher_response(session_id id, std::string& j_) {
    constexpr auto prefix = "rest_worker::on_dispatcher_response";
    ZoneScopedN(prefix);
    log_.trace("{}", prefix);
    auto j_str = std::move(j_);
    auto it = requests.find(id);
    if (it == requests.end()) {
        log_.error("{} unknown id:{}", prefix, id);
        return;
    }
    auto& req = it->second;
    log_.debug("{} id:{} mes:{}", prefix, id, j_str);
    rest_worker::init_resp_(req->create_response())
        .append_header(restinio::http_field::content_type, "text/json; charset=utf-8")
        .set_body(j_str)
        .done();
    requests.erase(id);
}
