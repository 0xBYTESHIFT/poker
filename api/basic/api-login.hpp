#pragma once
#include "api/json_obj.h"
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"

namespace api {

    struct login_request {
        static const inline prop_val<std::string> type{"type", "login_request"};
        prop_val<std::string> email{"email", ""};
        prop_val<std::string> pass_hash{"pass", ""};

        static auto from_json(const json& j) -> login_request;
        auto to_json() const -> json;
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

        static auto from_json(const json& j) -> login_response;
        auto to_json() const -> json;
    };

    inline auto login_request::from_json(const json& j) -> login_request {
        ZoneScoped;
        login_request req;
        read_from_json(j, req.email);
        read_from_json(j, req.pass_hash);
        return req;
    }
    inline auto login_request::to_json() const -> json {
        ZoneScoped;
        json j;
        write_to_json(j, email);
        write_to_json(j, pass_hash);
        return j;
    }
    inline auto login_response::from_json(const json& j) -> login_response {
        ZoneScoped;
        login_response rsp;
        read_from_json(j, rsp.message);
        read_from_json(j, rsp.token);
        read_from_json(j, rsp.code);
        return rsp;
    }
    inline auto login_response::to_json() const -> json {
        ZoneScoped;
        json j;
        write_to_json(j, message);
        write_to_json(j, token);
        write_to_json(j, code);
        return j;
    }
}; // namespace api
