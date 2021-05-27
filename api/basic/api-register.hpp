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

        static auto from_json(const json_t& j) -> register_request;
        auto to_json() const -> json_t;
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

        static auto from_json(const json_t& j) -> register_response;
        auto to_json() const -> json_t;
    };

    inline auto register_request::from_json(const json_t& j) -> register_request {
        ZoneScoped;
        register_request req;
        read_from_json(j, req.nickname);
        read_from_json(j, req.email);
        read_from_json(j, req.pass_hash);
        return req;
    }
    inline auto register_request::to_json() const -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, type);
        write_to_json(j, nickname);
        write_to_json(j, email);
        write_to_json(j, pass_hash);
        return j;
    }

    inline auto register_response::from_json(const json_t& j) -> register_response {
        ZoneScoped;
        register_response rsp;
        read_from_json(j, rsp.code);
        read_from_json(j, rsp.message);
        return rsp;
    }
    inline auto register_response::to_json() const -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, type);
        write_to_json(j, code);
        write_to_json(j, message);
        return j;
    }
}; // namespace api
