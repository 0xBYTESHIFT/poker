#pragma once
#include "api/json.hpp"
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"
#include <string>

namespace api {

    struct login_request {
        static const inline prop_val<std::string> type{"type", "login_request"};
        prop_val<std::string> email{"email", ""};
        prop_val<std::string> pass_hash{"pass", ""};

        login_request() = default;
        login_request(const json_t& j);
    };

    struct login_response {
        enum class code_enum : int {
            OK,
            NAME_UNKNOWN,
            PASS_INCORRECT,
            ETC
        };
        static const inline prop_val<std::string> type{"type", "login_response"};
        prop_val<std::string> message{"message", ""};
        prop_val<std::string> token{"token", ""};
        prop_val<code_enum> code{"code", code_enum::ETC};

        login_response() = default;
        login_response(const json_t& j);
    };

    inline login_request::login_request(const json_t& j) {
        ZoneScoped;
        read_from_json(j, email);
        read_from_json(j, pass_hash);
    }

    inline auto to_json(const login_request& req) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, req.type);
        write_to_json(j, req.email);
        write_to_json(j, req.pass_hash);
        return j;
    }

    inline login_response::login_response(const json_t& j) {
        ZoneScoped;
        read_from_json(j, message);
        read_from_json(j, token);
        read_from_json(j, code);
    }

    inline auto to_json(const login_response& rsp) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, rsp.type);
        write_to_json(j, rsp.message);
        write_to_json(j, rsp.token);
        write_to_json(j, rsp.code);
        return j;
    }
}; // namespace api
