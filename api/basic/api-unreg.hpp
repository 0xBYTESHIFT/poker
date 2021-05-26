#pragma once
#include "api/json_obj.h"
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"

namespace api {

    struct unregister_request {
        static const inline prop_val<std::string> type{"type", "unregister_request"};
        prop_val<std::string> email{"email", ""};
        prop_val<std::string> pass_hash{"pass_hash", ""};
        prop_val<std::string> token{"token", ""};

        static auto from_json(const json& j) -> unregister_request;
        auto to_json() const -> json;
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

        static auto from_json(const json& j) -> unregister_response;
        auto to_json() const -> json;
    };

    inline auto unregister_request::from_json(const json& j) -> unregister_request {
        ZoneScoped;
        unregister_request req;
        read_from_json(j, req.email);
        read_from_json(j, req.token);
        read_from_json(j, req.pass_hash);
        return req;
    }
    inline auto unregister_request::to_json() const -> json {
        ZoneScoped;
        json j;
        write_to_json(j, type);
        write_to_json(j, email);
        write_to_json(j, token);
        write_to_json(j, pass_hash);
        return j;
    }

    inline auto unregister_response::from_json(const json& j) -> unregister_response {
        ZoneScoped;
        unregister_response rsp;
        read_from_json(j, rsp.code);
        read_from_json(j, rsp.message);
        return rsp;
    }
    inline auto unregister_response::to_json() const -> json {
        ZoneScoped;
        json j;
        write_to_json(j, type);
        write_to_json(j, code);
        write_to_json(j, message);
        return j;
    }
}; // namespace api
