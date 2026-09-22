#ifndef XENIA_BASE_UI_SURFACE_ANDROID_H_
#define XENIA_BASE_UI_SURFACE_ANDROID_H_

#include <android/native_window.h>

#include <cstdint>

#include <rex/ui/surface.h>

namespace rex::ui {

class AndroidNativeWindowSurface final : public Surface {
 public:
  explicit AndroidNativeWindowSurface(ANativeWindow* window)
      : window_(window) {}

  TypeIndex GetType() const override {
    return kTypeIndex_AndroidNativeWindow;
  }

  ANativeWindow* window() const {
    return window_;
  }

 protected:
  bool GetSizeImpl(uint32_t& width_out,
                   uint32_t& height_out) const override;

 private:
  ANativeWindow* window_;
};

}  // namespace rex::ui

#endif  // XENIA_BASE_UI_SURFACE_ANDROID_H_
