#pragma once
#include "api/api.hpp"
#include <boost/algorithm/hex.hpp>
#include <boost/algorithm/string.hpp>
#include <cpr/cpr.h>
#include <openssl/ssl3.h>

#include <vector>

struct input_router {
    static auto hash(const std::string& orig) -> std::string {
        std::string result(SHA512_DIGEST_LENGTH, ' ');
        SHA512(reinterpret_cast<const unsigned char*>(orig.data()), orig.size(),
               reinterpret_cast<unsigned char*>(result.data()));
        return boost::algorithm::hex(result);
    }
    auto send(const std::string& remote, const std::string& j) -> cpr::Response {
        cpr::Response rsp = cpr::Get(cpr::Url{remote},
                                     cpr::Body{j},
                                     cpr::Header{{"Content-Type", "application/json"}});
        return rsp;
    }

    void run(const std::string& remote) {
        std::string line;
        std::string token, pass_hash, email;
        while (std::getline(std::cin, line)) {
            std::vector<std::string> words;
            boost::algorithm::split(words, line, boost::is_any_of(" "));
            if (words.empty()) {
                break;
            }
            auto& cmd = words.at(0);
            if (cmd == "/l") {
                api::login_request req;
                req.email = words.at(1);
                req.pass_hash = hash(words.at(2));
                email = req.email;
                pass_hash = req.pass_hash;
                auto rsp = send(remote, api::to_json(req));
                std::cout << rsp.error.message << " " << rsp.text << std::endl;
                auto rsp_ = api::from_json<api::login_response>(rsp.text);
                token = rsp_.token;
                continue;
            }
            if (cmd == "/r") {
                api::register_request req;
                req.nickname = words.at(1);
                req.pass_hash = hash(words.at(2));
                req.email = words.at(3);
                auto rsp = send(remote, req.to_json());
                std::cout << rsp.error.message << " " << rsp.text << std::endl;
                continue;
            }
            if (cmd == "/nr") {
                api::new_room_request req;
                req.token = token;
                req.pass_hash = pass_hash;
                auto rsp = send(remote, req.to_json());
                std::cout << rsp.error.message << " " << rsp.text << std::endl;
                continue;
            }
            if (cmd == "/lr") {
                api::list_rooms_request req;
                req.token = token;
                req.pass_hash = pass_hash;
                auto rsp = send(remote, req.to_json());
                std::cout << rsp.error.message << " " << rsp.text << std::endl;
                continue;
            }
            if (cmd == "/ur") {
                api::unregister_request req;
                req.email = email;
                req.pass_hash = pass_hash;
                req.token = token;
                auto rsp = send(remote, req.to_json());
                std::cout << rsp.error.message << " " << rsp.text << std::endl;
                continue;
            }
        }
    }
};