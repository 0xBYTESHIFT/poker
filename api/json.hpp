#pragma once
#include "components/log.hpp"
#include "components/tracy_include.hpp"
#include <boost/json.hpp>
#include <fmt/core.h>
#include <string>

using json_t = boost::json::object;

namespace json {
    using namespace boost::json;

    template<class T>
    auto read(const json_t& j, const std::string& name) {
        return value_to<T>(j.at(name));
    }

    template<class T>
    auto read(json_t& j, const std::string& name) {
        return value_to<T>(j.at(name));
    }

    template<class... Args>
    auto serialize(Args&&... args) {
        return boost::json::serialize(std::forward<Args>(args)...);
    }

    template<class Json>
    auto parse(const Json& j) {
        auto log = get_logger();
        auto prefix = "json::parse";
        log.debug("{} {}", prefix, j);
        return boost::json::parse(j).as_object();
    }

    template<class Json, class Ec>
    auto parse(const Json& j, Ec& ec) {
        auto log = get_logger();
        auto prefix = "json::parse";
        log.debug("{} {}", prefix, j);
        return boost::json::parse(j, ec).as_object();
    }
}; // namespace json
