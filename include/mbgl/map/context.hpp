#pragma once

#include <memory>

namespace mbgl {

class Backend;

namespace gl {
class Context;
} // namespace gl

class Context {
public:
    Context(std::unique_ptr<Backend>);
    ~Context();

    Backend& getBackend() { return *backend; }
    gl::Context& getGLContext() { return *context; }

private:
    std::unique_ptr<Backend> backend;
    std::unique_ptr<gl::Context> context;
};

} // namespace mbgl
