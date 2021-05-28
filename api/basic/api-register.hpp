#pragma once
#include "api/json.hpp"
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"

namespace api {
    struct register_request {
        static const inline prop_val<std::string> type{"type", "register_request"};
        prop_val<std::string> nickname{"nickname", ""};
        prop_val<std::string> email{"email", ""};
        prop_val<std::string> pass_hash{"pass", ""};

        register_request() = default;
        register_request(const json_t& j);
    };

    struct register_response {
        enum class code_enum : int {
            OK,
            NAME_TAKEN,
            ETC
        };
        static const inline prop_val<std::string> type{"type", "register_response"};
        prop_val<std::string> message{"message", ""};
        prop_val<code_enum> code{"code", code_enum::ETC};

        register_response() = default;
        register_response(const json_t& j);
    };

    inline register_request::register_request(const json_t& j) {
        ZoneScoped;
        read_from_json(j, nickname);
        read_from_json(j, email);
        read_from_json(j, pass_hash);
    }
    inline auto to_json(const register_request& req) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, req.type);
        write_to_json(j, req.nickname);
        write_to_json(j, req.email);
        write_to_json(j, req.pass_hash);
        return j;
    }

    inline register_response::register_response(const json_t& j) {
        ZoneScoped;
        read_from_json(j, code);
        read_from_json(j, message);
    }
    inline auto to_json(const register_response& rsp) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, rsp.type);
        write_to_json(j, rsp.code);
        write_to_json(j, rsp.message);
        return j;
    }
}; // namespace api
