#include "rex/ui/surface_android.h"

namespace rex::ui {

bool AndroidNativeWindowSurface::GetSizeImpl(
    uint32_t& width_out, uint32_t& height_out) const {
  width_out = static_cast<uint32_t>(ANativeWindow_getWidth(window_));
  height_out = static_cast<uint32_t>(ANativeWindow_getHeight(window_));
  return true;
}

}  // namespace rex::ui
