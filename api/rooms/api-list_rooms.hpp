#pragma once
#include "api/json_obj.h"
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"

namespace api {

    struct list_rooms_request {
        static const inline prop_val<std::string> type{"type", "list_rooms_request"};
        prop_val<std::string> token = {"token", ""};
        prop_val<std::string> pass_hash = {"pass_hash", ""};

        static auto from_json(const json& j) -> list_rooms_request;
        auto to_json() const -> json;
    };

    struct list_rooms_response {
        static const inline prop_val<std::string> type{"type", "list_rooms_response"};
        enum class code_enum {
            OK,
            TOKEN_INCORRECT,
            PASS_INCORRECT,
            ETC
        };
        prop_val<code_enum> code = {"code", code_enum::ETC};
        prop_val<std::string> message = {"message", ""};
        struct room_data_t {
            std::string name, token;
            size_t count;
        };
        using rooms_t = std::vector<room_data_t>;
        prop_val<rooms_t> rooms_data = {"rooms_data", rooms_t{}};

        static auto from_json(const json& j) -> list_rooms_response;
        auto to_json() const -> json;
    };

    inline auto list_rooms_request::from_json(const json& j) -> list_rooms_request {
        ZoneScoped;
        list_rooms_request req;
        read_from_json(j, req.token);
        read_from_json(j, req.pass_hash);
        return req;
    }
    inline auto list_rooms_request::to_json() const -> json {
        ZoneScoped;
        json j;
        write_to_json(j, this->type);
        write_to_json(j, this->token);
        write_to_json(j, this->pass_hash);
        return j;
    }

    /*==============================================================*/
    inline auto list_rooms_response::from_json(const json& j) -> list_rooms_response {
        ZoneScoped;
        list_rooms_response rsp;
        read_from_json(j, rsp.code);
        read_from_json(j, rsp.message);

        auto arr = j.inner_doc().FindMember(rsp.rooms_data().name())->value.GetArray();
        auto& rooms_data = rsp.rooms_data().downcast();
        rooms_data.reserve(arr.Size());
        for (auto& j_ : arr) {
            auto obj = j_.GetObject();
            room_data_t rd;
            rd.name = obj.FindMember("name")->value.GetString();
            rd.token = obj.FindMember("token")->value.GetString();
            rd.count = obj.FindMember("count")->value.GetInt64();
            rooms_data.emplace_back(rd);
        }
        return rsp;
    }
    inline auto list_rooms_response::to_json() const -> json {
        ZoneScoped;
        json j;
        write_to_json(j, this->type);
        write_to_json(j, this->code);
        write_to_json(j, this->message);
        std::vector<json> j_list;
        j_list.reserve(rooms_data()().size());
        for (auto& r_d : rooms_data()()) {
            json j_;
            j_["name"] = r_d.name;
            j_["token"] = r_d.token;
            j_["count"] = r_d.count;
            j_list.emplace_back(std::move(j));
        }
        j[rooms_data().name()] = std::move(j_list);
        return j;
    }

}; // namespace api
