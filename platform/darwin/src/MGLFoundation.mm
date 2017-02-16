#import "MGLFoundation_Private.h"

#include <mbgl/util/run_loop.hpp>

/// Initializes the run loop shim that lives on the main thread.
void MGLInitializeRunLoop() {
    static mbgl::util::RunLoop mainRunLoop;
}
