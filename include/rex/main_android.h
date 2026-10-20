#pragma once

#include <rex/platform.h>

#if REX_PLATFORM_ANDROID
#include <android/api-level.h>

namespace rex {

inline int GetAndroidApiLevel() {
  return android_get_device_api_level();
}

}  // namespace rex
#endif
