#include <mythos/core/application.hpp>
#include <mythos/log.hpp>

auto main(MYL_MAYBE_UNUSED int arg_count, MYL_MAYBE_UNUSED char** args) -> int {
    mye::log::init();
    
    std::unique_ptr<mye::application> app = mye::create_application();
    app->run();
    app.reset();

    return 0;
}
