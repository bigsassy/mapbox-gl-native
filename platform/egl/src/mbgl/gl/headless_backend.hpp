#pragma once

#include <mbgl/map/backend.hpp>

#include <memory>
#include <functional>

typedef void * EGLDisplay;
typedef void * EGLConfig;
typedef void * EGLSurface;
typedef void * EGLContext;

namespace mbgl {

class HeadlessBackend : public Backend {
public:
    HeadlessBackend();
    ~HeadlessBackend() override;

    void invalidate() override;
    void activate() override;
    void deactivate() override;
    void notifyMapChange(MapChange) override;

    void setMapChangeCallback(std::function<void(MapChange)>&& cb) { mapChangeCallback = std::move(cb); }

private:
    std::function<void(MapChange)> mapChangeCallback;

    EGLDisplay display = 0;
    EGLConfig config = 0;
    EGLContext glContext = 0;
    EGLSurface glSurface = 0;
};

} // namespace mbgl
