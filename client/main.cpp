#define BOOST_BIND_GLOBAL_PLACEHOLDERS //to silence compile warning
#include <csignal>
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/stacktrace.hpp>

#include "components/log.hpp"
#include "components/tracy_include.hpp"
#include "input_router.hpp"
#include <goblin-engineer/core.hpp>

void my_signal_handler(int signum) {
    ZoneScoped;
    ::signal(signum, SIG_DFL);
    std::cerr << "signal called:" << std::endl
              << boost::stacktrace::stacktrace() << std::endl;
    ::raise(SIGABRT);
}

void setup_handlers() {
    ZoneScoped;
    ::signal(SIGSEGV, &my_signal_handler);
    ::signal(SIGABRT, &my_signal_handler);
}

static auto original_terminate_handler{std::get_terminate()};

void terminate_handler() {
    ZoneScoped;
    std::cerr << "terminate called:" << std::endl
              << boost::stacktrace::stacktrace() << std::endl;
    original_terminate_handler();
    std::abort();
}

namespace po = boost::program_options;
namespace ge = goblin_engineer;

int main(int argc, char** argv) {
    ZoneScoped;
    setup_handlers();
    std::set_terminate(terminate_handler);

    auto log = initialization_logger();
    log.set_level(logger::level::info);
    constexpr auto prefix = "main";

    const auto port_name = "port";
    const auto host_name = "host";
    const auto num_workers_name = "num_workers";
    const auto max_throughput_name = "max_throughput";

    po::options_description desc("Allowed options");
    desc.add_options()("help", "this message");
    desc.add_options()(host_name, po::value<std::string>(), "address of host.\n");
    desc.add_options()(port_name, po::value<std::string>(), "port of host.\n");
    desc.add_options()(num_workers_name, po::value<unsigned int>(), "max workers for actors.\n");
    desc.add_options()(max_throughput_name, po::value<unsigned int>(), "max throughput for actors\n");
    desc.add_options()("verbose", po::value<int>(),
                       "level of verbosity.\n"
                       "0 - trace\n"
                       "1 - debug\n"
                       "2 - info\n"
                       "3 - warn\n"
                       "4 - err\n"
                       "5 - critical\n"
                       "6 - off\n");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::stringstream ss;
        ss << desc;
        std::string mes = ss.str();
        log.info(mes);
        return 0;
    }

    if (vm.count("verbose")) {
        int verb = vm["verbose"].as<int>();
        int max = (int) logger::level::level_nums;
        if (verb >= max) {
            auto msg = fmt::format("max alloved verbocity level:{0}", max);
            log.error(msg);
            throw std::runtime_error(msg);
        }
        log.set_level(static_cast<logger::level>(verb));
    }

    std::string remote_host;
    unsigned int num_worker = 2, max_throughput_param = 1000;

    auto lbd_get_str = [](po::variables_map& vm, const std::string& name, std::string& var, bool mand = false) {
        if (vm.count(name)) {
            var = vm[name].as<std::string>();
        } else if (mand) {
            auto mes = fmt::format("parameter --{} is mandatory!", name);
            throw std::runtime_error(mes);
        }
    };
    auto lbd_get_int = [](po::variables_map& vm, const std::string& name, unsigned int& var, bool mand = false) {
        if (vm.count(name)) {
            var = vm[name].as<unsigned int>();
        } else if (mand) {
            auto mes = fmt::format("parameter --{} is mandatory!", name);
            throw std::runtime_error(mes);
        }
    };

    lbd_get_str(vm, host_name, remote_host, true);
    lbd_get_int(vm, num_workers_name, num_worker);
    lbd_get_int(vm, max_throughput_name, max_throughput_param);

    log.info("Verbose: {}", (int) log.get_level());
    log.debug("Debug logging test");

    using wrk_shared = actor_zeta::executor_t<actor_zeta::work_sharing>;
    boost::asio::io_context context;
    //auto coordinator = coord_t(new wrk_shared(1, max_throughput_param), detail::thread_pool_deleter());
    //auto mngr = ge::make_manager_service<manager>(coordinator);

    auto sigint_set = std::make_shared<boost::asio::signal_set>(context, SIGINT, SIGTERM);
    sigint_set->async_wait(
        [sigint_set](const boost::system::error_code& /*err*/, int /*num*/) {
            ZoneScoped;
            sigint_set->cancel();
        });

    std::thread ctx_thr([&]() { context.run(); });
    ctx_thr.detach();
    input_router rt;
    rt.run(remote_host);
    context.stop();
    return 0;
}
