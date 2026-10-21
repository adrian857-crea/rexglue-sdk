
#include <rex/platform.h>

#if REX_PLATFORM_ANDROID

#include <rex/filesystem.h>

#include <SDL3/SDL_system.h>
#include <jni.h>

#include <string>

namespace rex::filesystem {

bool IsAndroidContentUri(const std::string_view source) {
  return source.starts_with("content://");
}

int OpenAndroidContentFileDescriptor(
    const std::string_view uri, const char* mode) {
  if (!IsAndroidContentUri(uri) || !mode) {
    return -1;
  }

  auto* env = static_cast<JNIEnv*>(SDL_GetAndroidJNIEnv());
  if (!env || env->PushLocalFrame(16) < 0) {
    return -1;
  }

  int result = -1;

  do {
    auto activity =
        static_cast<jobject>(SDL_GetAndroidActivity());
    if (!activity || env->ExceptionCheck()) break;

    jclass activity_class = env->GetObjectClass(activity);
    if (!activity_class || env->ExceptionCheck()) break;

    jmethodID get_resolver = env->GetMethodID(
        activity_class, "getContentResolver",
        "()Landroid/content/ContentResolver;");
    if (!get_resolver || env->ExceptionCheck()) break;

    jobject resolver =
        env->CallObjectMethod(activity, get_resolver);
    if (!resolver || env->ExceptionCheck()) break;

    jclass uri_class =
        env->FindClass("android/net/Uri");
    if (!uri_class || env->ExceptionCheck()) break;

    jmethodID parse_uri = env->GetStaticMethodID(
        uri_class, "parse",
        "(Ljava/lang/String;)Landroid/net/Uri;");
    if (!parse_uri || env->ExceptionCheck()) break;

    std::string uri_string(uri);
    jstring java_uri = env->NewStringUTF(uri_string.c_str());
    jstring java_mode = env->NewStringUTF(mode);
    if (!java_uri || !java_mode || env->ExceptionCheck()) break;

    jobject parsed_uri =
        env->CallStaticObjectMethod(uri_class, parse_uri, java_uri);
    if (!parsed_uri || env->ExceptionCheck()) break;

    jclass resolver_class = env->GetObjectClass(resolver);
    if (!resolver_class || env->ExceptionCheck()) break;

    jmethodID open_descriptor = env->GetMethodID(
        resolver_class, "openFileDescriptor",
        "(Landroid/net/Uri;Ljava/lang/String;)"
        "Landroid/os/ParcelFileDescriptor;");
    if (!open_descriptor || env->ExceptionCheck()) break;

    jobject descriptor = env->CallObjectMethod(
        resolver, open_descriptor, parsed_uri, java_mode);
    if (!descriptor || env->ExceptionCheck()) break;

    jclass descriptor_class = env->GetObjectClass(descriptor);
    if (!descriptor_class || env->ExceptionCheck()) break;

    jmethodID detach_fd = env->GetMethodID(
        descriptor_class, "detachFd", "()I");
    if (!detach_fd || env->ExceptionCheck()) break;

    result = env->CallIntMethod(descriptor, detach_fd);
    if (env->ExceptionCheck()) result = -1;

  } while (false);

  if (env->ExceptionCheck()) {
    env->ExceptionClear();
  }

  env->PopLocalFrame(nullptr);
  return result;
}

}  // namespace rex::filesystem

#endif  // REX_PLATFORM_ANDROID
