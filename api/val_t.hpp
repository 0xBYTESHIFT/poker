#pragma once
#include "api/json.hpp"
#include "components/property.hpp"
#include "components/tracy_include.hpp"
#include <string>

namespace api {

    template<class T>
    struct val_t final {
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
        std::string name_;
        T val_;
    };

    template<class T>
    using prop_val = property<val_t<T>>;

    template<class T>
    void read_from_json(const json_t& j, prop_val<T>& val) requires std::is_convertible_v<T, std::string_view> {
        val()() = boost::json::value_to<std::string>(j.at(val().name()));
    }

    template<class T>
    void read_from_json(const json_t& j, prop_val<T>& val) requires std::floating_point<T> {
        val()() = boost::json::value_to<double>(j.at(val().name()));
    }

    template<class T>
    void read_from_json(const json_t& j, prop_val<T>& val) requires std::integral<T> {
        val()() = boost::json::value_to<int>(j.at(val().name()));
    }

    template<class T>
    void read_from_json(const json_t& j, prop_val<T>& val) {
        val()() = static_cast<T>(boost::json::value_to<int>(j.at(val().name())));
    }

    template<class T>
        void write_to_json(json_t& j, const prop_val<T>& val) requires std::floating_point<T> || std::integral<T> || std::is_convertible_v<T, std::string_view> {
        j[val().name()] = val()();
    }

    template<class T>
    void write_to_json(json_t& j, const prop_val<T>& val) {
        j[val().name()] = static_cast<int>(val()());
    }

    template<class T>
    void extract(const boost::json::object& obj, T& t, std::string_view key) {
        //t = value_to<T>(obj.at(key));
    }

    /*
    template<class T>
    auto tag_invoke(value_to_tag<prop_val<T>>, const boost::json::value& jv) -> prop_val<T>{
        customer c;
        object const& obj = jv.as_object();
        extract(obj, c.id, "id");
        extract(obj, c.name, "name");
        extract(obj, c.current, "current");
        return c;
    }
    */
}; // namespace api
