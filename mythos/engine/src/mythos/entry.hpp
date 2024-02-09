#include <mythos/core/application.hpp>
#include <mythos/log.hpp>

#include <mythos/assert.hpp>
#include <myl/macro.hpp>

#include <format>

auto main(MYL_MAYBE_UNUSED int arg_count, MYL_MAYBE_UNUSED char** args) -> int {
    myth::log::init();
    
    auto app = myth::create_application();
    app->run();
    
    return 0;
}
