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

        static auto from_json(const json_t& j) -> unlogin_request;
        auto to_json() const -> json_t;
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

        static auto from_json(const json_t& j) -> unlogin_response;
        auto to_json() const -> json_t;
    };

    inline auto unlogin_request::from_json(const json_t& j) -> unlogin_request {
        ZoneScoped;
        unlogin_request req;
        read_from_json(j, req.email);
        read_from_json(j, req.token);
        read_from_json(j, req.pass_hash);
        return req;
    }
    inline auto unlogin_request::to_json() const -> json_t{
        ZoneScoped;
        json_t j;
        write_to_json(j, type);
        write_to_json(j, email);
        write_to_json(j, token);
        write_to_json(j, pass_hash);
        return j;
    }

    inline auto unlogin_response::from_json(const json_t& j) -> unlogin_response {
        ZoneScoped;
        unlogin_response rsp;
        read_from_json(j, rsp.code);
        read_from_json(j, rsp.message);
        return rsp;
    }
    inline auto unlogin_response::to_json() const -> json_t{
        ZoneScoped;
        json_t j;
        write_to_json(j, type);
        write_to_json(j, code);
        write_to_json(j, message);
        return j;
    }
}; // namespace api
