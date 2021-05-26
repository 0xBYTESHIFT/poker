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
            auto rsp = send(remote, req.to_json());
            const auto reg_rsp = api::register_response::from_json(rsp.text);
            if (reg_rsp.code != api::register_response::code_enum::OK) {
                throw std::runtime_error("reg");
            }
        }
        {
            api::login_request req;
            req.email = mail;
            req.pass_hash = pass;
            auto rsp = send(remote, to_json(req));
            auto log_rsp = api::from_json<api::login_response>(rsp.text);
            if (log_rsp.code != api::login_response::code_enum::OK) {
                throw std::runtime_error("login");
            }
            token = log_rsp.token;
        }
        {
            api::unlogin_request req;
            req.email = mail;
            req.pass_hash = pass;
            req.token = token;
            auto rsp = send(remote, req.to_json());
            auto unlog_rsp = api::unlogin_response::from_json(rsp.text);
            if (unlog_rsp.code != api::unlogin_response::code_enum::OK) {
                throw std::runtime_error("unlogin");
            }
        }
        {
            api::unregister_request req;
            req.email = mail;
            req.pass_hash = pass;
            auto rsp = send(remote, req.to_json());
            auto unreg_rsp = api::unregister_response::from_json(rsp.text);
            if (unreg_rsp.code != api::unregister_response::code_enum::OK) {
                throw std::runtime_error("unreg");
            }
        }
        //std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}
