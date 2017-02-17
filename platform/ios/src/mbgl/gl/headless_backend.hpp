#pragma once

#include <mbgl/map/backend.hpp>

#include <OpenGLES/EAGL.h>

#include <memory>
#include <functional>

namespace mbgl {

class HeadlessBackend : public Backend {
public:
    HeadlessBackend();

    void invalidate() override;
    void activate() override;
    void deactivate() override;
    ProcAddress getProcAddress(const char * name) override;

    void notifyMapChange(MapChange) override;
    void setMapChangeCallback(std::function<void(MapChange)>&& cb) { mapChangeCallback = std::move(cb); }

private:
    std::function<void(MapChange)> mapChangeCallback;

    EAGLContext* glContext = nullptr;
};

} // namespace mbgl
