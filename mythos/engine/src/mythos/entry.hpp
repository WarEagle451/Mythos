#include <mythos/core/application.hpp>
#include <mythos/log.hpp>

auto main(MYL_MAYBE_UNUSED int arg_count, MYL_MAYBE_UNUSED char** args) -> int {
    /// MYTODO: Implement below
    /// myth::application_specification app_specs{};
    /// myth::set_application_specification(&app_specs);

    myth::log::init();
    
    std::unique_ptr<myth::application> app = myth::create_application();
    app->run();
    app.reset();

    return 0;
}
