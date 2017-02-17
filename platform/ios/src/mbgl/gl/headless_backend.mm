#include <mbgl/gl/headless_backend.hpp>

#include <cassert>
#include <stdexcept>
#include <string>

namespace mbgl {

HeadlessBackend::HeadlessBackend() {
    glContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    if (glContext == nil) {
        throw std::runtime_error("Error creating GL context object");
    }

    glContext.multiThreaded = YES;
}

void HeadlessBackend::activate() {
    [EAGLContext setCurrentContext:glContext];
}

void HeadlessBackend::deactivate() {
    [EAGLContext setCurrentContext:nil];
}

void HeadlessBackend::invalidate() {
    assert(false);
}

Backend::ProcAddress HeadlessBackend::getProcAddress(const char * name) {
    static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengles"));
    if (!framework) {
        throw std::runtime_error("Failed to load OpenGL framework.");
    }

    CFStringRef str = CFStringCreateWithCString(kCFAllocatorDefault, name, kCFStringEncodingASCII);
    void* symbol = CFBundleGetFunctionPointerForName(framework, str);
    CFRelease(str);

    return reinterpret_cast<ProcAddress>(symbol);
}

void HeadlessBackend::notifyMapChange(MapChange change) {
    if (mapChangeCallback) {
        mapChangeCallback(change);
    }
}

} // namespace mbgl
