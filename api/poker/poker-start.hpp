#pragma once
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"
#include "rapidjson/prettywriter.h"

namespace api {
    using str_t = std::string;

    struct poker_start_request {
        static const inline prop_val<str_t> type{"type", "poker_start_request"};
        prop_val<str_t> token = {"token", ""};
        prop_val<str_t> pass_hash = {"pass_hash", ""};

        poker_start_request(const json_t& j);
        auto to_json(const poker_start_request& req) -> json_t;
    };

    struct poker_start_response {
        static const inline prop_val<str_t> type{"type", "poker_start_response"};
        enum class code_enum {
            OK,
            TOKEN_INCORRECT,
            PASS_INCORRECT,
            ROOM_TOKEN_INCORRECT,
            USER_NOT_IN_THE_ROOM,
            ETC
        };
        prop_val<code_enum> code = {"code", code_enum::ETC};
        prop_val<str_t> message = {"message", ""};

        poker_start_response(const json_t& j);
        auto to_json(const poker_start_response& rsp) -> json_t;
    };

    inline poker_start_request::poker_start_request(const json_t& j) {
        ZoneScoped;
        read_from_json(j, token);
        read_from_json(j, pass_hash);
    }
    inline auto to_json(const poker_start_request& req) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, req.type);
        write_to_json(j, req.token);
        write_to_json(j, req.pass_hash);
        return j;
    }

    inline poker_start_response::poker_start_response(const json_t& j) {
        ZoneScoped;
        read_from_json(j, code);
        read_from_json(j, message);
    }
    inline auto to_json(const poker_start_response& rsp) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, rsp.type);
        write_to_json(j, rsp.code);
        write_to_json(j, rsp.message);
        return j;
    }
}; // namespace api
