#pragma once
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"
#include "rapidjson/prettywriter.h"

namespace api {

    struct enter_room_request {
        static const inline prop_val<std::string> type{"type", "enter_room_request"};
        prop_val<std::string> token = {"token", ""};
        prop_val<std::string> pass_hash = {"pass_hash", ""};
        prop_val<std::string> room_token = {"room_token", ""};

        enter_room_request() = default;
        enter_room_request(const json_t& j);
    };

    struct enter_room_response {
        enum class code_enum {
            OK,
            TOKEN_INCORRECT,
            PASS_INCORRECT,
            ROOM_TOKEN_INCORRECT,
            ETC
        };
        static const inline prop_val<std::string> type{"type", "enter_room_response"};
        prop_val<code_enum> code = {"code", code_enum::ETC};
        prop_val<std::string> message = {"message", ""};

        enter_room_response() = default;
        enter_room_response(const json_t& j);
    };

    inline enter_room_request::enter_room_request(const json_t& j) {
        ZoneScoped;
        read_from_json(j, token);
        read_from_json(j, pass_hash);
        read_from_json(j, room_token);
    }
    inline auto to_json(const enter_room_request& req) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, req.type);
        write_to_json(j, req.token);
        write_to_json(j, req.room_token);
        write_to_json(j, req.pass_hash);
        return j;
    }

    inline enter_room_response::enter_room_response(const json_t& j) {
        ZoneScoped;
        read_from_json(j, code);
        read_from_json(j, message);
    }
    inline auto to_json(const enter_room_response& rsp) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, rsp.type);
        write_to_json(j, rsp.message);
        write_to_json(j, rsp.code);
        return j;
    }

}; // namespace api
