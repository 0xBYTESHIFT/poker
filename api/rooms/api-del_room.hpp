#pragma once
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"
#include "rapidjson/prettywriter.h"

namespace api {

    struct del_room_request {
        static const inline prop_val<std::string> type{"type", "del_room_request"};
        prop_val<std::string> token = {"token", ""};
        prop_val<std::string> pass_hash = {"pass_hash", ""};
        prop_val<std::string> room_token = {"room_token", ""};

        static auto from_json(const json& j) -> del_room_request;
        auto to_json() const -> json;
    };

    struct del_room_response {
        enum class code_enum {
            OK,
            TOKEN_INCORRECT,
            PASS_INCORRECT,
            ROOM_TOKEN_INCORRECT,
            NO_RIGHTS,
            ROOM_NOT_EMPTY,
            ETC
        };
        static const inline prop_val<std::string> type{"type", "del_room_response"};
        prop_val<code_enum> code = {"code", code_enum::ETC};
        prop_val<std::string> message = {"message", ""};

        static auto from_json(const json& j) -> del_room_response;
        auto to_json() const -> json;
    };

    inline auto del_room_request::from_json(const json& j) -> del_room_request {
        ZoneScoped;
        del_room_request req;
        read_from_json(j, req.token);
        read_from_json(j, req.room_token);
        read_from_json(j, req.pass_hash);
        return req;
    }
    inline auto del_room_request::to_json() const -> json {
        ZoneScoped;
        json j;
        write_to_json(j, this->type);
        write_to_json(j, this->token);
        write_to_json(j, this->room_token);
        write_to_json(j, this->pass_hash);
        return j;
    }

    /*==============================================================*/
    inline auto del_room_response::from_json(const json& j) -> del_room_response {
        ZoneScoped;
        del_room_response rsp;
        read_from_json(j, rsp.code);
        read_from_json(j, rsp.message);
        return rsp;
    }
    inline auto del_room_response::to_json() const -> json {
        ZoneScoped;
        json j;
        write_to_json(j, this->type);
        write_to_json(j, this->message);
        write_to_json(j, this->code);
        return j;
    }

}; // namespace api
