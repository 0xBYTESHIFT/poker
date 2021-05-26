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
        prop_val<str_t> room_token = {"room_token", ""};

        static auto from_json(const json& j) -> leave_room_request;
        auto to_json() const -> str_t;
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

        static auto from_json(const json& j) -> leave_room_response;
        auto to_json() const -> str_t;
    };

    inline auto poker_start_request::from_json(const json& j) -> poker_start_request {
        ZoneScoped;
        poker_start_request r;
        r.token() = j.value_as<str_t>(r.token().name());
        r.room_token() = j.value_as<str_t>(r.room_token().name());
        r.pass_hash() = j.value_as<str_t>(r.pass_hash().name());
        return r;
    }
    inline auto poker_start_request::to_json() const -> str_t {
        ZoneScoped;
        str_t result;
        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<decltype(sb)> pw(sb);

        pw.StartObject();
        write_to_json(pw, this->type);
        write_to_json(pw, this->token);
        write_to_json(pw, this->room_token);
        write_to_json(pw, this->pass_hash);
        pw.EndObject();

        result = sb.GetString();
        return result;
    }

    /*==============================================================*/
    inline auto poker_start_response::from_json(const json& j) -> poker_start_request {
        ZoneScoped;
        poker_start_response r;
        auto temp = j.value_as<std::size_t>(r.code().name());
        r.code() = static_cast<leave_room_response::code_enum>(temp);
        r.message() = j.value_as<str_t>(r.message().name());
        return r;
    }
    inline auto poker_start_response::to_json() const -> str_t {
        ZoneScoped;
        str_t result;
        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<decltype(sb)> pw(sb);

        pw.StartObject();
        write_to_json(pw, this->type);
        write_to_json(pw, this->message);

        pw.String(this->code().name());
        pw.Int64(size_t(this->code().downcast()));
        pw.EndObject();

        result = sb.GetString();
        return result;
    }

}; // namespace api
