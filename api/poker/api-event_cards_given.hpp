#pragma once
#include "api/val_t.hpp"
#include "components/tracy_include.hpp"
#include "rapidjson/prettywriter.h"

namespace api {
    using str_t = std::string;

    struct event_poker_cards_given {
        static const inline prop_val<str_t> type{"type", "event_poker_cards_given"};

        event_poker_cards_given(const json_t& j);
        auto to_json(const event_poker_cards_given& req) -> json_t;
    };

    inline event_poker_cards_given::event_poker_cards_given(const json_t& j) {
        ZoneScoped;
    }
    inline auto to_json(const event_poker_cards_given& req) -> json_t {
        ZoneScoped;
        json_t j;
        write_to_json(j, req.type);
        return j;
    }
}; // namespace api
