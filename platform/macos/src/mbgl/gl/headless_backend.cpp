#include <mbgl/gl/headless_backend.hpp>

#include <OpenGL/OpenGL.h>
#include <CoreFoundation/CoreFoundation.h>

#include <cassert>
#include <stdexcept>
#include <string>

namespace mbgl {

HeadlessBackend::HeadlessBackend() {
    // TODO: test if OpenGL 4.1 with GL_ARB_ES2_compatibility is supported
    // If it is, use kCGLOGLPVersion_3_2_Core and enable that extension.
    CGLPixelFormatAttribute attributes[] = {
        kCGLPFAOpenGLProfile, static_cast<CGLPixelFormatAttribute>(kCGLOGLPVersion_Legacy),
        // Not adding kCGLPFAAccelerated, as this will make headless rendering impossible when running in VMs.
        kCGLPFAClosestPolicy,
        kCGLPFAAccumSize, static_cast<CGLPixelFormatAttribute>(32),
        kCGLPFAColorSize, static_cast<CGLPixelFormatAttribute>(24),
        kCGLPFAAlphaSize, static_cast<CGLPixelFormatAttribute>(8),
        kCGLPFADepthSize, static_cast<CGLPixelFormatAttribute>(16),
        kCGLPFAStencilSize, static_cast<CGLPixelFormatAttribute>(8),
        kCGLPFASupportsAutomaticGraphicsSwitching,
        kCGLPFAAllowOfflineRenderers, // Allows using the integrated GPU
        static_cast<CGLPixelFormatAttribute>(0)
    };

    GLint num;
    CGLError error = CGLChoosePixelFormat(attributes, &pixelFormat, &num);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Error choosing pixel format:") + CGLErrorString(error) + "\n");
    }
    if (num <= 0) {
        throw std::runtime_error("No pixel formats found.");
    }

    error = CGLCreateContext(pixelFormat, nullptr, &glContext);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Error creating GL context object:") +
                                 CGLErrorString(error) + "\n");
    }

    error = CGLEnable(glContext, kCGLCEMPEngine);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Error enabling OpenGL multithreading:") +
                                 CGLErrorString(error) + "\n");
    }
}

HeadlessBackend::~HeadlessBackend() {
    CGLDestroyContext(glContext);
    CGLDestroyPixelFormat(pixelFormat);
}

void HeadlessBackend::activate() {
    CGLError error = CGLSetCurrentContext(glContext);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Switching OpenGL context failed:") +
                                 CGLErrorString(error) + "\n");
    }
}

void HeadlessBackend::deactivate() {
    CGLError error = CGLSetCurrentContext(nullptr);
    if (error != kCGLNoError) {
        throw std::runtime_error(std::string("Removing OpenGL context failed:") +
                                 CGLErrorString(error) + "\n");
    }
}

void HeadlessBackend::invalidate() {
    assert(false);
}

Backend::ProcAddress HeadlessBackend::getProcAddress(const char * name) {
    static CFBundleRef framework = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.opengl"));
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
