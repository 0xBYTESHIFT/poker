#pragma once
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"
#include "rapidjson/prettywriter.h"

namespace api {

    struct new_room_request {
        static const inline prop_val<std::string> type{"type", "new_room_request"};
        prop_val<std::string> token = {"token", ""};
        prop_val<std::string> pass_hash = {"pass_hash", ""};
        //prop_val<std::string> room_pass_hash = {"room_pass_hash", ""};

        new_room_request() = default;
        new_room_request(const json_t& j);
    };

    struct new_room_response {
        static const inline prop_val<std::string> type{"type", "new_room_response"};
        enum class code_enum {
            OK,
            TOKEN_INCORRECT,
            PASS_INCORRECT,
            ROOM_PASS_INCORRECT,
            ETC
        };
        prop_val<code_enum> code = {"code", code_enum::ETC};
        prop_val<std::string> message = {"message", ""};
        prop_val<std::string> room_token = {"room_token", ""};

        new_room_response() = default;
        new_room_response(const json_t& j);
    };

    inline new_room_request::new_room_request(const json_t& j) {
        ZoneScoped;
        read_from_json(j, token);
        read_from_json(j, pass_hash);
        //read_from_json(j, req.room_pass_hash);
    }
    inline auto to_json(const new_room_request& req) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, req.type);
        write_to_json(j, req.token);
        write_to_json(j, req.pass_hash);
        return j;
    }

    inline new_room_response::new_room_response(const json_t& j) {
        ZoneScoped;
        read_from_json(j, code);
        read_from_json(j, message);
        read_from_json(j, room_token);
    }
    inline auto to_json(const new_room_response& rsp) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, rsp.type);
        write_to_json(j, rsp.code);
        write_to_json(j, rsp.message);
        write_to_json(j, rsp.room_token);
        return j;
    }

}; // namespace api
