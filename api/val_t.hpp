#pragma once
#include "components/property.hpp"
#include "components/tracy_include.hpp"
#include <string>
#include "api/json_obj.h"

namespace api {

    template<class T>
    struct val_t {
        using value_t = T;

        val_t() = delete;

        template<class... Args>
        val_t(const std::string& name, Args&&... args)
            : name_(name)
            , val_(std::forward<Args>(args)...) {}

        auto operator=(const T& arg) -> val_t& {
            this->val_ = arg;
            return *this;
        }

        auto operator()() const -> const T& { return val_; }
        auto operator()() -> T& { return val_; }

        auto downcast() const -> const T& { return val_; }
        auto downcast() -> T& { return val_; }

        operator T() { return val_; }
        operator T() const { return val_; }

        auto name() const -> const std::string& { return name_; }

    protected:
        const std::string name_;
        T val_;
    };

    template<class T>
    using prop_val = property<val_t<T>>;

    template<class Json, class T>
    void read_from_json(const Json& j, prop_val<T>& val) requires std::is_convertible_v<T, std::string_view> {
        val()() = j[val().name()].GetString();
    }

    template<class Json, class T>
    void read_from_json(const Json& j, prop_val<T>& val) requires std::floating_point<T> {
        val()() = j[val().name()].GetDouble();
    }

    template<class Json, class T>
    void read_from_json(const Json& j, prop_val<T>& val) requires std::integral<T> {
        val()() = j[val().name()].GetInt();
    }

    template<class Json, class T>
    void read_from_json(const Json& j, prop_val<T>& val) {
        val()() = static_cast<T>(j[val().name()].GetInt());
    }

    template<class Json, class T>
    void write_to_json(Json& j, const prop_val<T>& val) requires std::floating_point<T> || std::integral<T> || std::is_convertible_v<T, std::string_view> {
        ZoneScoped;
        j[val().name()] = val()();
    }

    template<class Json, class T>
    void write_to_json(Json& j, const prop_val<T>& val) {
        ZoneScoped;
        j[val().name()] = static_cast<int>(val()());
    }

    template<class Json, class T>
    void write_to_json(Json& j, const std::string& name, const std::vector<T>& vec) {
        ZoneScoped;
        rapidjson::StringBuffer sb;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> wr(sb);
        wr.String(name);
        wr.StartArray();
        for(auto &v:vec){
        }
        wr.EndArray();
        j["name"] = json(sb.GetString()).inner_doc();
    }
}; // namespace api
