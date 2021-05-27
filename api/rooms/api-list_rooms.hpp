#pragma once
#include "api/json.hpp"
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"

namespace api {

    struct list_rooms_request {
        static const inline prop_val<std::string> type{"type", "list_rooms_request"};
        prop_val<std::string> token = {"token", ""};
        prop_val<std::string> pass_hash = {"pass_hash", ""};

        static auto from_json(const json_t& j) -> list_rooms_request;
        auto to_json() const -> json_t;
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
            prop_val<std::string> name{"name"};
            prop_val<std::string> token{"token"};
            prop_val<size_t> count{"count"};
        };
        using rooms_t = std::vector<room_data_t>;
        prop_val<rooms_t> rooms_data = {"rooms_data", rooms_t{}};

        static auto from_json(const json_t& j) -> list_rooms_response;
        auto to_json() const -> json_t;
    };

    inline auto list_rooms_request::from_json(const json_t& j) -> list_rooms_request {
        ZoneScoped;
        list_rooms_request req;
        read_from_json(j, req.token);
        read_from_json(j, req.pass_hash);
        return req;
    }
    inline auto list_rooms_request::to_json() const -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, this->type);
        write_to_json(j, this->token);
        write_to_json(j, this->pass_hash);
        return j;
    }

    /*==============================================================*/
    inline auto list_rooms_response::from_json(const json_t& j) -> list_rooms_response {
        ZoneScoped;
        list_rooms_response rsp;
        read_from_json(j, rsp.code);
        read_from_json(j, rsp.message);

        auto& arr = j.at(rsp.rooms_data().name()).as_array();
        auto& rooms_data = rsp.rooms_data().downcast();
        rooms_data.reserve(arr.size());
        for (auto& j_ : arr) {
            room_data_t rd;
            read_from_json(j_.as_object(), rd.token);
            read_from_json(j_.as_object(), rd.name);
            read_from_json(j_.as_object(), rd.count);
            rooms_data.emplace_back(rd);
        }
        return rsp;
    }
    inline auto list_rooms_response::to_json() const -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, this->type);
        write_to_json(j, this->code);
        write_to_json(j, this->message);
        std::vector<boost::json::object> j_list;
        j_list.reserve(rooms_data()().size());
        for (auto& r_d : rooms_data()()) {
            json_t j_;
            write_to_json(j_, r_d.name);
            write_to_json(j_, r_d.token);
            write_to_json(j_, r_d.count);
            j_list.emplace_back(std::move(j));
        }
        j[rooms_data().name()] = boost::json::value_from(j_list);
        return j;
    }

}; // namespace api
