#include "api/api.hpp"
#include "components/log.hpp"
#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <cpr/cpr.h>
#include <openssl/ssl3.h>
#include <random>
#include <thread>

auto send(const std::string& remote, const std::string& j) -> cpr::Response {
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
    if (argc < 3) {
        throw std::runtime_error("provide target and count!");
    }
    auto log = initialization_logger();
    log.set_level(logger::level::debug);
    std::string remote = argv[1];
    auto count = std::stoull(argv[2]);
    auto nick = random_string(20);
    auto pass = hash(random_string(20));
    auto mail = random_string(20);
    std::string token;
    {
        api::register_request req;
        req.nickname = nick;
        req.pass_hash = pass;
        req.email = mail;
        const auto rsp = send(remote, req.to_json());
        log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
        const auto reg_rsp = api::register_response::from_json(json::parse(rsp.text));
        if (reg_rsp.code()() != api::register_response::code_enum::OK) {
            throw std::runtime_error("register");
        }
    }
    {
        api::login_request req;
        req.email = mail;
        req.pass_hash = pass;
        const auto rsp = send(remote, req.to_json());
        log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
        const auto log_rsp = api::login_response::from_json(json::parse(rsp.text));
        if (log_rsp.code()() != api::login_response::code_enum::OK) {
            throw std::runtime_error("login");
        }
        token = log_rsp.token()();
    }
    for (size_t i = 0; i < count; i++) {
        std::string room_token;
        {
            api::new_room_request req;
            req.token = token;
            req.pass_hash = pass;
            const auto rsp = send(remote, req.to_json());
            log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
            const auto room_rsp = api::new_room_response::from_json(json::parse(rsp.text));
            if (room_rsp.code() != api::new_room_response::code_enum::OK) {
                throw std::runtime_error("new room");
            }
            room_token = room_rsp.room_token().downcast();
        }
        {
            api::enter_room_request req;
            req.token = token;
            req.pass_hash = pass;
            req.room_token = room_token;
            const auto rsp = send(remote, req.to_json());
            log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
            const auto room_rsp = api::enter_room_response::from_json(json::parse(rsp.text));
            if (room_rsp.code() != api::enter_room_response::code_enum::OK) {
                throw std::runtime_error("enter room");
            }
        }
        {
            api::leave_room_request req;
            req.token = token;
            req.pass_hash = pass;
            req.room_token = room_token;
            const auto rsp = send(remote, req.to_json());
            log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
            const auto room_rsp = api::leave_room_response::from_json(json::parse(rsp.text));
            if (room_rsp.code().downcast() != api::leave_room_response::code_enum::OK) {
                throw std::runtime_error("leave room");
            }
        }
        {
            api::del_room_request req;
            req.token = token;
            req.pass_hash = pass;
            req.room_token = room_token;
            const auto rsp = send(remote, req.to_json());
            log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
            const auto room_rsp = api::del_room_response::from_json(json::parse(rsp.text));
            //room was deleted automatically
            if (room_rsp.code() != api::del_room_response::code_enum::ROOM_TOKEN_INCORRECT) {
                throw std::runtime_error("del room");
            }
        }
    }
    {
        api::unregister_request req;
        req.email = mail;
        req.pass_hash = pass;
        const auto rsp = send(remote, req.to_json());
        log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
        const auto unreg_rsp = api::unregister_response::from_json(json::parse(rsp.text));
        if (unreg_rsp.code() != api::unregister_response::code_enum::OK) {
            throw std::runtime_error("unreg");
        }
    }
    {
        api::unlogin_request req;
        req.email = mail;
        req.pass_hash = pass;
        req.token = token;
        const auto rsp = send(remote, req.to_json());
        log.debug("reg rsp:{} code:{} er:{} er_mes:{}", rsp.text, rsp.status_code, rsp.error.code, rsp.error.message);
        const auto unlog_rsp = api::unlogin_response::from_json(json::parse(rsp.text));
        if (unlog_rsp.code() != api::unlogin_response::code_enum::OK) {
            throw std::runtime_error("unlogin");
        }
    }
}
