#include <android/asset_manager_jni.h>
#include <android/native_window_jni.h>
#include "FastAndFurious.h"

static FastAndFurious app;

#ifdef __cplusplus
extern "C" {
#endif

jint JNI_OnLoad(JavaVM *vm, void *) {
  // We need to store a reference to the Java VM so that we can call back
  app.SetJavaVM(vm);
  return JNI_VERSION_1_6;
}

// Takes care of saving asset path and all
JNIEXPORT void JNICALL
Java_reality_escher_com_fastandfurious_MainActivity_onCreateJNI(
    JNIEnv *env, jobject clazz, jobject activity, jobject j_asset_manager) {
  app.OnCreate(env, activity);
  app.SetAssetManager(AAssetManager_fromJava(env, j_asset_manager));
}

JNIEXPORT void JNICALL
Java_reality_escher_com_fastandfurious_MainActivity_blurToggle(
    JNIEnv *env, jobject clazz) {
  app.BlurToggle();
}

JNIEXPORT void JNICALL
Java_reality_escher_com_fastandfurious_MainActivity_fastToggle(
    JNIEnv *env, jobject clazz) {
  app.FastToggle();
}

JNIEXPORT void JNICALL
Java_reality_escher_com_fastandfurious_MainActivity_flipCamera(
    JNIEnv *env, jobject clazz) {
  app.HaltCamera();
}

// Alot of stuff depends on the m_frame_buffer being loaded
// this is done in SetNativeWindow
JNIEXPORT void JNICALL
Java_reality_escher_com_fastandfurious_MainActivity_setSurface(JNIEnv *env, jclass clazz, jobject surface) {

  // obtain a native window from a Java surface
  app.SetNativeWindow(ANativeWindow_fromSurface(env, surface));

  // Set camera parameters up
  app.SetUpCamera();

  std::thread loopThread(&FastAndFurious::CameraLoop, &app);
  loopThread.detach();
}

#ifdef __cplusplus
}
#endif