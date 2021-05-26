#pragma once
#include <goblin-engineer/core.hpp>
#include <actor-zeta/actor-zeta.hpp>

namespace detail {
    struct thread_pool_deleter final {
        void operator()(actor_zeta::abstract_executor* ptr) {
            ptr->stop();
            delete ptr;
        }
    };
} // namespace detail


using coord_t = std::unique_ptr<actor_zeta::abstract_executor, detail::thread_pool_deleter>;