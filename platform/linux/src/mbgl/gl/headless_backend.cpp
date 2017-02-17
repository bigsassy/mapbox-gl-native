#include <mbgl/gl/headless_backend.hpp>
#include <mbgl/util/logging.hpp>

#include <cassert>
#include <stdexcept>
#include <string>

namespace mbgl {

HeadlessBackend::HeadlessBackend() {
    if (!XInitThreads()) {
        throw std::runtime_error("Failed to XInitThreads.");
    }

    xDisplay = XOpenDisplay(nullptr);
    if (xDisplay == nullptr) {
        throw std::runtime_error("Failed to open X display.");
    }

    const char *extensions = reinterpret_cast<const char *>(glXQueryServerString(xDisplay, DefaultScreen(xDisplay), GLX_EXTENSIONS));
    if (!extensions) {
        throw std::runtime_error("Cannot read GLX extensions.");
    }
    if (!strstr(extensions,"GLX_SGIX_fbconfig")) {
        throw std::runtime_error("Extension GLX_SGIX_fbconfig was not found.");
    }
    if (!strstr(extensions, "GLX_SGIX_pbuffer")) {
        throw std::runtime_error("Cannot find glXCreateContextAttribsARB.");
    }

    // We're creating a dummy pbuffer anyway that we're not using.
    static int pixelFormat[] = {
        GLX_DRAWABLE_TYPE, GLX_PBUFFER_BIT,
        None
    };

    int configs = 0;
    fbConfigs = glXChooseFBConfig(xDisplay, DefaultScreen(xDisplay), pixelFormat, &configs);
    if (fbConfigs == nullptr) {
        throw std::runtime_error("Failed to glXChooseFBConfig.");
    }
    if (configs <= 0) {
        throw std::runtime_error("No Framebuffer configurations.");
    }

    // Try to create a legacy context.
    glContext = glXCreateNewContext(xDisplay, fbConfigs[0], GLX_RGBA_TYPE, None, True);

    if (glContext && !glXIsDirect(xDisplay, glContext)) {
        Log::Error(Event::OpenGL, "failed to create direct OpenGL Legacy context");
        glXDestroyContext(xDisplay, glContext);
        glContext = nullptr;
    }

    if (glContext == nullptr) {
        throw std::runtime_error("Error creating GL context object.");
    }

    // Create a dummy pbuffer. We will render to framebuffers anyway, but we need a pbuffer to
    // activate the context.
    int pbufferAttributes[] = {
        GLX_PBUFFER_WIDTH, 8,
        GLX_PBUFFER_HEIGHT, 8,
        None
    };
    glxPbuffer = glXCreatePbuffer(xDisplay, fbConfigs[0], pbufferAttributes);
}

HeadlessBackend::~HeadlessBackend() {
    glXDestroyPbuffer(xDisplay, glxPbuffer);
    glXDestroyContext(xDisplay, glContext);
    XFree(fbConfigs);
    XCloseDisplay(xDisplay);
}

void HeadlessBackend::activate() {
    if (!glXMakeContextCurrent(xDisplay, glxPbuffer, glxPbuffer, glContext)) {
        throw std::runtime_error("Switching OpenGL context failed.\n");
    }
}

void HeadlessBackend::deactivate() {
    if (!glXMakeContextCurrent(xDisplay, 0, 0, nullptr)) {
        throw std::runtime_error("Removing OpenGL context failed.\n");
    }
}

void HeadlessBackend::invalidate() {
    assert(false);
}

Backend::ProcAddress HeadlessBackend::getProcAddress(const char * name) {
    return glXGetProcAddress(reinterpret_cast<const GLubyte*>(name));
}

void HeadlessBackend::notifyMapChange(MapChange change) {
    if (mapChangeCallback) {
        mapChangeCallback(change);
    }
}

} // namespace mbgl
