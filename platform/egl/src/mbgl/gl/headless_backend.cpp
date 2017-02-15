#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/gl/extension.hpp>
#include <mbgl/util/logging.hpp>

#include <EGL/egl.h>

#include <cassert>
#include <stdexcept>
#include <string>

namespace mbgl {

HeadlessBackend::HeadlessBackend() {
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        throw std::runtime_error("Failed to obtain a valid EGL display.\n");
    }

    EGLint major, minor, numConfigs;
    if (!eglInitialize(display, &major, &minor)) {
        throw std::runtime_error("eglInitialize() failed.\n");
    }

    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglBindAPI(EGL_OPENGL_ES_API) returned error %d", eglGetError());
        throw std::runtime_error("eglBindAPI() failed");
    }

    const EGLint configAttribs[] = {
#if __ANDROID__
        // Android emulator requires a pixel buffer to generate renderable unit
        // test results.
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
#endif // __ANDROID__
        EGL_NONE
    };

    if (!eglChooseConfig(display, configAttribs, &config, 1, &numConfigs) || numConfigs != 1) {
        throw std::runtime_error("Failed to choose ARGB config.\n");
    }

    // EGL initializes the context client version to 1 by default. We want to
    // use OpenGL ES 2.0 which has the ability to create shader and program
    // objects and also to write vertex and fragment shaders in the OpenGL ES
    // Shading Language.
    const EGLint contextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    glContext = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    if (glContext == EGL_NO_CONTEXT) {
        mbgl::Log::Error(mbgl::Event::OpenGL, "eglCreateContext() returned error 0x%04x",
                         eglGetError());
        throw std::runtime_error("Error creating the EGL context object.\n");
    }

#if __ANDROID__
    // Create a pixel buffer surface (in conjunction with EGL_SURFACE_TYPE, EGL_PBUFFER_BIT).
    const EGLint surfAttribs[] = {
        EGL_WIDTH, 512,
        EGL_HEIGHT, 512,
        EGL_LARGEST_PBUFFER, EGL_TRUE,
        EGL_NONE
    };

    glSurface = eglCreatePbufferSurface(display, config, surfAttribs);
    if (glSurface == EGL_NO_SURFACE) {
        throw std::runtime_error("Could not create surface: " + std::to_string(eglGetError()));
    }
#endif // __ANDROID__

    activate();
    gl::InitializeExtensions(eglGetProcAddress);
}

HeadlessBackend::~HeadlessBackend() {
#if __ANDROID__
    if (!eglDestroySurface(display, glSurface)) {
        throw std::runtime_error("Failed to destroy EGL context.\n");
    }
#endif // __ANDROID__

    if (!eglDestroyContext(display, glContext)) {
        throw std::runtime_error("Failed to destroy EGL context.\n");
    }

    eglTerminate(display);
}

void HeadlessBackend::activate() {
    if (!eglMakeCurrent(display, glSurface, glSurface, glContext)) {
        throw std::runtime_error("Switching OpenGL context failed.\n");
    }
}

void HeadlessBackend::deactivate() {
    if (!eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT)) {
        throw std::runtime_error("Removing OpenGL context failed.\n");
    }
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
