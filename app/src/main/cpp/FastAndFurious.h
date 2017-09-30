#ifndef FASTANDFURIOUS_FASTANDFURIOUS_H
#define FASTANDFURIOUS_FASTANDFURIOUS_H

// Android
#include <android/asset_manager.h>
#include <android/native_window.h>
#include <jni.h>
// OpenCL
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/opencl.h>
#define PI_ 3.14159265359f
// FastAndFurious
#include "Util.h"
#include "ImageReader.h"
#include "NativeCamera.h"
// C Libs
#include <unistd.h>
#include <math.h>
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

  // sets Asset Manager reference pointer
  void SetAssetManager(AAssetManager* asset_manager) { m_aasset_manager = asset_manager; };

  // Gets all the stuff needed to setup camera and calls loop
  void SetUpCamera();

  // TODO - make functions private

  // Loops camera on seperate thread
  void CameraLoop();

  // Destroys current session and flips
  void FlipCamera();

  // Stops camera active
  // This will cause CameraLoop to break and call FlipCamera
  // This is needed due to being on seperate thread
  void HaltCamera();

  void GaussianBlur_init();
  void GaussianBlur(ANativeWindow_Buffer* buffer);
  void GaussianBlur_CPU(ANativeWindow_Buffer* buffer);

  void BlurToggle(){ m_blur_mode = !m_blur_mode; };
  void FastToggle(){ m_fast_mode = !m_fast_mode; };

  cl_mem cl_create_buffer(cl_context context,
                          cl_mem_flags flags,
                          size_t size,
                          void *host_ptr);

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

  bool m_camera_thread_stopped = false;

  // Toggle bool to turn of/off features
  bool m_blur_mode;
  bool m_fast_mode;

  cl_platform_id gb_platform;        // OpenCL platform
  cl_device_id gb_device;           // device ID
  cl_context gb_context;               // context
  cl_command_queue gb_queue;           // command queue
  cl_program gb_program;               // program
  void* gb_kernel_file;                // kernal char* of text
  cl_kernel gb_kernel;                 // kernel
  int gb_blur_sum = 61;
  int gb_blur_dim = 5;

//  int gb_filter[25] = {1,4,7,4,1,
//                       4,16,26,16,4,
//                       7,26,41,26,7,
//                       4,16,26,16,4,
//                       1,4,7,4,1};

  int gb_filter[25] = {4,3,2,3,4, //16
                      3,2,1,2,3,  //11
                      2,1,1,1,2,  //7
                      3,2,1,2,3,  //11
                      4,3,2,3,4}; //16


};

#endif //FASTANDFURIOUS_FASTANDFURIOUS_H
