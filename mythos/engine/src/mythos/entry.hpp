#include <mythos/core/application.hpp>
#include <mythos/log.hpp>

auto main(int arg_count, char** args) -> int {
    myth::log::init();
    
    auto app = myth::create_application();
    app->run();
    
    return 0;
}
