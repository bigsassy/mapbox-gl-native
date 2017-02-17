#include <mbgl/gl/extension.hpp>
#include <mbgl/gl/gl.hpp>
#include <mbgl/map/backend.hpp>

#include <mutex>
#include <string>
#include <vector>
#include <cstring>

namespace mbgl {
namespace gl {
namespace detail {

using Probes = std::vector<ExtensionFunctionBase::Probe>;
using ExtensionFunctions = std::vector<std::pair<glProc*, Probes>>;
ExtensionFunctions& extensionFunctions() {
    static ExtensionFunctions functions;
    return functions;
}

ExtensionFunctionBase::ExtensionFunctionBase(std::initializer_list<Probe> probes) {
    extensionFunctions().emplace_back(&ptr, probes);
}

} // namespace detail

static std::once_flag initializeExtensionsOnce;

void InitializeExtensions(Backend& backend) {
    std::call_once(initializeExtensionsOnce, [&backend] {
        if (const char* extensions =
                reinterpret_cast<const char*>(MBGL_CHECK_ERROR(glGetString(GL_EXTENSIONS)))) {
            for (auto fn : detail::extensionFunctions()) {
                for (auto probe : fn.second) {
                    if (strstr(extensions, probe.first) != nullptr) {
                        *fn.first = backend.getProcAddress(probe.second);
                        break;
                    }
                }
            }
        }
    });
}

} // namespace gl
} // namespace mbgl
