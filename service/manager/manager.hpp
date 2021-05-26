#pragma once
#include "components/tracy_include.hpp"
#include "components/log.hpp"
#include "coordinator/coord.hpp"
#include <goblin-engineer/core.hpp>

namespace ge = goblin_engineer;

class manager final : public ge::abstract_manager_service
{
public:
    //using base_t = ge::basic_manager_service_t<proc_manager_async_policy>;
    using addr = ge::actor_address;

    explicit manager(coord_t &coord);
    ~manager();

    auto executor() noexcept -> ge::abstract_executor * final override;
    auto get_executor() noexcept -> ge::abstract_executor * final override;
    auto enqueue_base(ge::message_ptr msg, actor_zeta::execution_device *) -> void override;

    using ge::abstract_manager_service::addresses;
    using ge::abstract_manager_service::all_view_address;

private:
    logger log_;
    coord_t &e_;
};

inline manager::manager(coord_t &coord)
    : ge::abstract_manager_service("manager"), e_(coord)
{
    ZoneScoped;
    e_->start();
}

inline manager::~manager()
{
    ZoneScoped;
    e_->stop();
}

inline auto manager::executor() noexcept -> ge::abstract_executor * { return e_.get(); }
inline auto manager::get_executor() noexcept -> ge::abstract_executor * { return e_.get(); }

inline auto manager::enqueue_base(ge::message_ptr msg, actor_zeta::execution_device *) -> void
{
    ZoneScoped;
    set_current_message(std::move(msg));
    execute(*this);
}