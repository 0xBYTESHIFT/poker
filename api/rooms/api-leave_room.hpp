#pragma once
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"
#include "rapidjson/prettywriter.h"

namespace api {

    struct leave_room_request {
        static const inline prop_val<std::string> type{"type", "leave_room_request"};
        prop_val<std::string> token = {"token", ""};
        prop_val<std::string> pass_hash = {"pass_hash", ""};
        prop_val<std::string> room_token = {"room_token", ""};

        static auto from_json(const json_t& j) -> leave_room_request;
        auto to_json() const -> json_t;
    };

    struct leave_room_response {
        static const inline prop_val<std::string> type{"type", "leave_room_response"};
        enum class code_enum {
            OK,
            TOKEN_INCORRECT,
            PASS_INCORRECT,
            ROOM_TOKEN_INCORRECT,
            USER_NOT_IN_THE_ROOM,
            ETC
        };
        prop_val<code_enum> code = {"code", code_enum::ETC};
        prop_val<std::string> message = {"message", ""};

        static auto from_json(const json_t& j) -> leave_room_response;
        auto to_json() const -> json_t;
    };

    inline auto leave_room_request::from_json(const json_t& j) -> leave_room_request {
        ZoneScoped;
        leave_room_request req;
        read_from_json(j, req.token);
        read_from_json(j, req.room_token);
        read_from_json(j, req.pass_hash);
        return req;
    }
    inline auto leave_room_request::to_json() const -> json_t{
        ZoneScoped;
        json_t j;
        write_to_json(j, this->type);
        write_to_json(j, this->token);
        write_to_json(j, this->room_token);
        write_to_json(j, this->pass_hash);
        return j;
    }

    inline auto leave_room_response::from_json(const json_t& j) -> leave_room_response {
        ZoneScoped;
        leave_room_response rsp;
        read_from_json(j, rsp.code);
        read_from_json(j, rsp.message);
        return rsp;
    }
    inline auto leave_room_response::to_json() const -> json_t{
        ZoneScoped;
        json_t j;
        write_to_json(j, this->type);
        write_to_json(j, this->message);
        write_to_json(j, this->code);
        return j;
    }

}; // namespace api
