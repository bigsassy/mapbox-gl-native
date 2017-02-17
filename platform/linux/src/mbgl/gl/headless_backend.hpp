#pragma once

#include <mbgl/map/backend.hpp>

#include <GL/glx.h>

#include <memory>
#include <functional>

namespace mbgl {

class HeadlessBackend : public Backend {
public:
    HeadlessBackend();
    ~HeadlessBackend() override;

    void invalidate() override;
    void activate() override;
    void deactivate() override;
    ProcAddress getProcAddress(const char * name) override;

    void notifyMapChange(MapChange) override;
    void setMapChangeCallback(std::function<void(MapChange)>&& cb) { mapChangeCallback = std::move(cb); }

private:
    std::function<void(MapChange)> mapChangeCallback;

    Display* xDisplay = nullptr;
    GLXFBConfig* fbConfigs = nullptr;
    GLXContext glContext = nullptr;
    GLXPbuffer glxPbuffer = 0;
};

} // namespace mbgl
