#include <mythos/core/application.hpp>
#include <mythos/core/log.hpp>
#include <mythos/version.hpp>

auto main(MYL_MAYBE_UNUSED int arg_count, MYL_MAYBE_UNUSED char** args) -> int {
    myth::log::init();
    MYTHOS_INFO("Mythos:");
    MYTHOS_INFO("\t- Version: {}", MYTHOS_VERSION_STRING);

    auto app = myth::create_application();
    app->run();
    
    return 0;
}
