#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/extension.hpp>

#include <QGLContext>

#if QT_VERSION >= 0x050000
#include <QOpenGLContext>
#endif

#include <cassert>

namespace mbgl {

HeadlessBackend::HeadlessBackend() {
    activate();
    gl::InitializeExtensions([] (const char * name) {
#if QT_VERSION >= 0x050000
        QOpenGLContext* thisContext = QOpenGLContext::currentContext();
        return thisContext->getProcAddress(name);
#else
        const QGLContext* thisContext = QGLContext::currentContext();
        return reinterpret_cast<mbgl::gl::glProc>(thisContext->getProcAddress(name));
#endif
    });
}

void HeadlessBackend::activate() {
    widget.makeCurrent();
}

void HeadlessBackend::deactivate() {
    widget.doneCurrent();
}

void HeadlessBackend::invalidate() {
    assert(false);
}

void HeadlessBackend::notifyMapChange(MapChange change) {
    if (mapChangeCallback) {
        mapChangeCallback(change);
    }
}

} // namespace mbgl
