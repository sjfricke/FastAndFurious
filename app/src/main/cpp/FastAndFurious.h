#ifndef FASTANDFURIOUS_FASTANDFURIOUS_H
#define FASTANDFURIOUS_FASTANDFURIOUS_H

// Android
#include <android/asset_manager.h>
#include <android/native_window.h>
#include <jni.h>
// OpenCV-NDK App
#include "Util.h"
#include "ImageReader.h"
#include "NativeCamera.h"
// C Libs
#include <unistd.h>
#include <time.h>
// STD Libs
#include <cstdlib>
#include <string>
#include <vector>
#include <thread>

class FastAndFurious {
 public:
  FastAndFurious();
  ~FastAndFurious();
  FastAndFurious(const FastAndFurious& other) = delete;
  FastAndFurious& operator=(const FastAndFurious& other) = delete;

  // Lets us know when app has started passing in VM info
  void OnCreate(JNIEnv* env, jobject caller_activity);

  // TODO
  // Disconnect from service
  void OnPause();

  // TODO
  // Cleanup
  void OnDestroy();

  // Cache the Java VM used from the Java layer.
  void SetJavaVM(JavaVM* pjava_vm) { java_vm = pjava_vm; }

  // sets Surface buffer reference pointer
  void SetNativeWindow(ANativeWindow* native_indow);

  // sets Surface buffer reference pointer
  void SetAssetManager(AAssetManager* asset_manager) {
    m_aasset_manager = asset_manager;
  };

  // Gets all the stuff needed to setup camera and calls loop
  void SetUpCamera();

  // TODO - make functions private

  // Loops camera on seperate thread
  void CameraLoop();

  // Destroys current session and flips
  void FlipCamera();

  // Stops camera active
  void HaltCamera();

  void BlurToggle(){ m_blur_mode = !m_blur_mode; };
  void FastToggle(){ m_fast_mode = !m_fast_mode; };


 private:
  // Cached Java VM, caller activity object
  JavaVM* java_vm;
  jobject calling_activity_obj;
  jmethodID on_callback;

  // holds native window to write buffer too
  ANativeWindow* m_native_window;

  // buffer to hold native window when writing to it
  ANativeWindow_Buffer m_native_buffer;

  // Camera variables
  NativeCamera* m_native_camera;

  camera_type m_selected_camera_type = BACK_CAMERA; // Default

  // Image Reader
  ImageFormat m_view{0, 0, 0};
  ImageReader* m_image_reader;
  AImage* m_image;

  volatile bool m_camera_ready;

  // used to hold reference to assets in assets folder
  AAssetManager* m_aasset_manager;

  // for timing OpenCV bottlenecks
  clock_t start_t, end_t;
  double  total_t;

  bool m_camera_thread_stopped = false;

  // Toggle bool to turn of/off features
  bool m_blur_mode;
  bool m_fast_mode;
};

#endif //FASTANDFURIOUS_FASTANDFURIOUS_H
