#define BOOST_BIND_GLOBAL_PLACEHOLDERS //to silence compile warning
#include "api/api.hpp"
#include "components/log.hpp"
#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/stacktrace.hpp>
#include <chrono>
#include <cpr/cpr.h>
#include <csignal>
#include <openssl/ssl3.h>
#include <random>
#include <thread>

void my_signal_handler(int signum) {
    ZoneScoped;
    ::signal(signum, SIG_DFL);
    std::cerr << "signal called:" << std::endl
              << boost::stacktrace::stacktrace() << std::endl;
    ::raise(SIGABRT);
}

void setup_handlers() {
    ZoneScoped;
    ::signal(SIGSEGV, &my_signal_handler);
    ::signal(SIGABRT, &my_signal_handler);
}

static auto original_terminate_handler{std::get_terminate()};

void terminate_handler() {
    ZoneScoped;
    std::cerr << "terminate called:" << std::endl
              << boost::stacktrace::stacktrace() << std::endl;
    original_terminate_handler();
    std::abort();
}

auto send(const std::string& remote, const std::string& j) -> cpr::Response {
    auto log = get_logger();
    auto prefix = "send(str)";
    log.debug("{} {}", prefix, j);
    cpr::Response rsp = cpr::Get(cpr::Url{remote},
                                 cpr::Body{j},
                                 cpr::Header{{"Content-Type", "application/json"}});
    return rsp;
}

auto send(const std::string& remote, const json_t& j) -> cpr::Response {
    return send(remote, json::serialize(j));
}

static auto hash(const std::string& orig) -> std::string {
    std::string result(SHA512_DIGEST_LENGTH, ' ');
    SHA512(reinterpret_cast<const unsigned char*>(orig.data()), orig.size(),
           reinterpret_cast<unsigned char*>(result.data()));
    return boost::algorithm::hex(result);
}

auto random_string(size_t size) {
    std::string alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, alphabet.size() - 1);
    std::string result;
    result.resize(size, '\0');
    for (size_t i = 0; i < size; i++) {
        result[i] = alphabet[dist(gen)];
    }
    return result;
}

int main(int argc, char* argv[]) {
    setup_handlers();
    std::set_terminate(terminate_handler);
    if (argc < 3) {
        throw std::runtime_error("provide target and count!");
    }
    auto log = initialization_logger();
    log.set_level(logger::level::trace);
    std::string remote = argv[1];
    auto count = std::stoull(argv[2]);
    for (size_t i = 0; i < count; i++) {
        auto nick = random_string(20);
        auto pass = hash(random_string(20));
        auto mail = random_string(20);
        std::string token;

        {
            api::register_request req;
            req.nickname = nick;
            req.pass_hash = pass;
            req.email = mail;
            const auto rsp = send(remote, api::to_json(req));
            log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
            const auto reg_rsp = api::register_response(json::parse(rsp.text));
            if (reg_rsp.code() != api::register_response::code_enum::OK) {
                throw std::runtime_error("reg");
            }
        }
        {
            api::login_request req;
            req.email = mail;
            req.pass_hash = pass;
            const auto rsp = send(remote, api::to_json(req));
            log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
            auto log_rsp = api::login_response(json::parse(rsp.text));
            if (log_rsp.code() != api::login_response::code_enum::OK) {
                throw std::runtime_error("login");
            }
            token = log_rsp.token()();
        }
        {
            api::unlogin_request req;
            req.email = mail;
            req.pass_hash = pass;
            req.token = token;
            const auto rsp = send(remote, api::to_json(req));
            log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
            auto unlog_rsp = api::unlogin_response(json::parse(rsp.text));
            if (unlog_rsp.code() != api::unlogin_response::code_enum::OK) {
                throw std::runtime_error("unlogin");
            }
        }
        {
            api::unregister_request req;
            req.email = mail;
            req.pass_hash = pass;
            const auto rsp = send(remote, api::to_json(req));
            log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
            auto unreg_rsp = api::unregister_response(json::parse(rsp.text));
            if (unreg_rsp.code() != api::unregister_response::code_enum::OK) {
                throw std::runtime_error("unreg");
            }
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
