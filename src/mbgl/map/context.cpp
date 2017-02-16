#include <mbgl/map/context.hpp>
#include <mbgl/map/backend.hpp>
#include <mbgl/map/backend_scope.hpp>
#include <mbgl/gl/context.hpp>

namespace mbgl {

Context::Context(std::unique_ptr<Backend> backend_)
    : backend(std::move(backend_)),
      context(std::make_unique<gl::Context>()) {
}

Context::~Context() {
    BackendScope scope(*backend);
    context.reset();
}

} // namespace mbgl
