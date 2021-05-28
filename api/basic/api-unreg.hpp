#pragma once
#include "api/json.hpp"
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"

namespace api {

    struct unregister_request {
        static const inline prop_val<std::string> type{"type", "unregister_request"};
        prop_val<std::string> email{"email", ""};
        prop_val<std::string> pass_hash{"pass_hash", ""};
        prop_val<std::string> token{"token", ""};

        unregister_request() = default;
        unregister_request(const json_t& j);
    };

    struct unregister_response {
        enum class code_enum : int {
            OK,
            NO_NAME,
            NO_PASS,
            TOKEN_INCORRECT,
            ETC
        };
        static const inline prop_val<std::string> type{"type", "unregister_response"};
        prop_val<std::string> message{"message", ""};
        prop_val<code_enum> code{"code", code_enum::ETC};

        unregister_response() = default;
        unregister_response(const json_t& j);
    };

    inline unregister_request::unregister_request(const json_t& j) {
        ZoneScoped;
        read_from_json(j, email);
        read_from_json(j, token);
        read_from_json(j, pass_hash);
    }
    inline auto to_json(const unregister_request& req) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, req.type);
        write_to_json(j, req.email);
        write_to_json(j, req.token);
        write_to_json(j, req.pass_hash);
        return j;
    }

    inline unregister_response::unregister_response(const json_t& j) {
        ZoneScoped;
        read_from_json(j, code);
        read_from_json(j, message);
    }
    inline auto to_json(const unregister_response& rsp) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, rsp.type);
        write_to_json(j, rsp.code);
        write_to_json(j, rsp.message);
        return j;
    }
}; // namespace api
