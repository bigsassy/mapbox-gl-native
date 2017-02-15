#pragma once

#include <mbgl/map/backend.hpp>

#include <memory>
#include <functional>

typedef struct _CGLContextObject * CGLContextObj;
typedef struct _CGLPixelFormatObject * CGLPixelFormatObj;

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

    CGLPixelFormatObj pixelFormat = nullptr;
    CGLContextObj glContext = nullptr;
};

} // namespace mbgl
