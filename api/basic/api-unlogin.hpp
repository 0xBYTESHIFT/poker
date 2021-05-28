#pragma once
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"
#include "rapidjson/prettywriter.h"

namespace api {

    struct unlogin_request {
        static const inline prop_val<std::string> type{"type", "unlogin_request"};
        prop_val<std::string> email{"email", ""};
        prop_val<std::string> token{"token", ""};
        prop_val<std::string> pass_hash{"pass", ""};

        unlogin_request() = default;
        unlogin_request(const json_t& j);
    };

    struct unlogin_response {
        enum class code_enum : int {
            OK,
            NAME_UNKNOWN,
            PASS_INCORRECT,
            TOKEN_INCORRECT,
            ETC
        };
        static const inline prop_val<std::string> type{"type", "unlogin_request"};
        prop_val<code_enum> code{"code", code_enum::ETC};
        prop_val<std::string> message{"message", ""};

        unlogin_response() = default;
        unlogin_response(const json_t& j);
    };

    inline unlogin_request::unlogin_request(const json_t& j) {
        ZoneScoped;
        read_from_json(j, email);
        read_from_json(j, token);
        read_from_json(j, pass_hash);
    }
    inline auto to_json(const unlogin_request& req) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, req.type);
        write_to_json(j, req.email);
        write_to_json(j, req.token);
        write_to_json(j, req.pass_hash);
        return j;
    }

    inline unlogin_response::unlogin_response(const json_t& j) {
        ZoneScoped;
        read_from_json(j, code);
        read_from_json(j, message);
    }
    inline auto to_json(const unlogin_response& rsp) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, rsp.type);
        write_to_json(j, rsp.code);
        write_to_json(j, rsp.message);
        return j;
    }
}; // namespace api
