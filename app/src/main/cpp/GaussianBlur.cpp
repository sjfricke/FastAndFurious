#include "FastAndFurious.h"
#include <time.h>
#include <stdlib.h>

void FastAndFurious::GaussianBlur_init()
{
  cl_int err;

  err = clGetPlatformIDs(1, &gb_platform, NULL);
  if (err != CL_SUCCESS) { LOGE("error: clGetPlatformIDs() errcode %d", err); }

  /* get available device */
  err = clGetDeviceIDs(gb_platform, CL_DEVICE_TYPE_GPU, 1, &gb_device, NULL);
  if (err != CL_SUCCESS) { LOGE("error: clGetDeviceIDs() errcode %d", err); }

  /* create context */
  gb_context = clCreateContext(NULL, 1, &gb_device, NULL, NULL, &err);
  if (err != CL_SUCCESS) { LOGE("error: clCreateContext() errcode %d", err); }

  /* create comand queue */
  gb_queue = clCreateCommandQueue(gb_context, gb_device, 0, &err);
  if (err != CL_SUCCESS) { LOGE("error: clGetPlatformIDs() errcode %d", err); }

  /* create program source */
  const char* kernel_file = "GaussianBlur.cl";

  AAsset* asset = AAssetManager_open(m_aasset_manager, kernel_file, AASSET_MODE_BUFFER);
  if (asset == NULL) {
    LOGE("Error opening asset %s", kernel_file); exit(1);
  }

  off_t file_size = AAsset_getRemainingLength(asset);
  gb_kernel_file = malloc(file_size);

  int bytes_read = AAsset_read(asset, gb_kernel_file, file_size);
  if (static_cast<int>(file_size) != bytes_read) {
    LOGE("file_size: %d ==should=be== bytes_read: %d", static_cast<int>(file_size), bytes_read);
  }

  LOGI("%s", gb_kernel_file );

  gb_program = clCreateProgramWithSource(gb_context, 1, (const char **)&gb_kernel_file, NULL, &err);
  if (err != CL_SUCCESS) { LOGE("error: clGetPlatformIDs() errcode %d", err); }

  err = clBuildProgram(gb_program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS) { LOGE("error: clBuildProgram() errcode %d", err); }

  if (err == CL_BUILD_PROGRAM_FAILURE) {
    // Determine the size of the log
    size_t log_size;
    clGetProgramBuildInfo(gb_program, gb_device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

    // Allocate memory for the log
    char *log = (char *) malloc(log_size);

    // Get the log
    clGetProgramBuildInfo(gb_program, gb_device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);

    // Print the log
    LOGE("%s\n", log);
  }

}

void FastAndFurious::GaussianBlur(ANativeWindow_Buffer*  buffer)
{
  cl_mem gray_buf, blur_buf, gauss_buf;
  cl_kernel cl_img_gaussian_blur;
  cl_int err;
  size_t global_wblur[2];
  size_t local_wblur[2];
  int len;

  len = buffer->stride * buffer->height * 4;

  err = 0;

  cl_img_gaussian_blur = clCreateKernel(gb_program, "cl_img_gaussian_blur", &err);
  if (err != CL_SUCCESS) { LOGE("error: clCreateKernel() errcode %d", err); }

  gray_buf = cl_create_buffer(gb_context,  CL_MEM_READ_ONLY, len, NULL);
  gauss_buf = cl_create_buffer(gb_context, CL_MEM_READ_ONLY, gb_blur_dim*gb_blur_dim*sizeof(cl_int), NULL);
  /* output buffer */
  blur_buf = cl_create_buffer(gb_context, CL_MEM_WRITE_ONLY, len, NULL);

  err |= clSetKernelArg(cl_img_gaussian_blur, 0, sizeof(cl_mem), &gray_buf);
  err |= clSetKernelArg(cl_img_gaussian_blur, 1, sizeof(cl_mem), &blur_buf);
  err |= clSetKernelArg(cl_img_gaussian_blur, 2, sizeof(cl_mem), &gauss_buf);
  err |= clSetKernelArg(cl_img_gaussian_blur, 3, sizeof(cl_int), &gb_blur_dim);
  err |= clSetKernelArg(cl_img_gaussian_blur, 4, sizeof(cl_int), &gb_blur_sum);
  err |= clSetKernelArg(cl_img_gaussian_blur, 5, sizeof(cl_int), &buffer->width);
  err |= clSetKernelArg(cl_img_gaussian_blur, 6, sizeof(cl_int), &buffer->height);
  err |= clSetKernelArg(cl_img_gaussian_blur, 7, sizeof(cl_int), &buffer->stride);

  if (err != CL_SUCCESS) { LOGE("error: clSetKernelArg() errcode %d", err); }

  err |= clEnqueueWriteBuffer(gb_queue, gray_buf, CL_FALSE, 0, len, buffer->bits, 0, NULL, NULL);
  err |= clEnqueueWriteBuffer(gb_queue, gauss_buf, CL_FALSE, 0, gb_blur_dim*gb_blur_dim*sizeof(cl_int), &gb_filter, 0, NULL, NULL);

   if (err != CL_SUCCESS) { LOGE("error: clEnqueueWriteBuffer() errcode %d", err); }

  global_wblur[0] = buffer->height;
  global_wblur[1] = buffer->stride;
  local_wblur[0] = local_wblur[1] = 32;

  //LOGI("ABOUT TO RUN ON GPU");

  err = clEnqueueNDRangeKernel(gb_queue, cl_img_gaussian_blur, 2, NULL, global_wblur, local_wblur, 0, NULL, NULL);
   if (err != CL_SUCCESS) { LOGE("error: clEnqueueNDRangeKernel() errcode %d", err); }

  //LOGI("DONE ON GPU");

  err = clEnqueueReadBuffer(gb_queue, blur_buf, CL_TRUE, 0, len, buffer->bits, 0, NULL, NULL);
  if (err != CL_SUCCESS) { LOGE("error: clEnqueueReadBuffer() errcode %d", err); }

  //LOGI("RETURN MEM FROM GPU");

  clReleaseMemObject(gray_buf);
  //LOGI("Rleaese 1");
  clReleaseMemObject(blur_buf);
  //LOGI("Rleaese 2");
  clReleaseMemObject(gauss_buf);
  //LOGI("Rleaese 3");
  clReleaseKernel(cl_img_gaussian_blur);
  //LOGI("Rleaese 4");

}

//void FastAndFurious::GaussianBlur_CPU(ANativeWindow_Buffer *buffer) {
////
////  uint32_t size = 5;
////  float sigma = .8;
////  uint32_t i,x,y,imgLineSize;
////  int32_t center,yOff,xOff;
////  float* matrix,value;
////  matrix = GaussianBlur_BlurKernel(size,sigma);
////
////  //find the size of one line of the image in bytes and the center of the gaussian kernel
////  imgLineSize = buffer->stride * 4;
////  center = size/2;
////  //convolve all valid pixels with the gaussian kernel
////  for(i = imgLineSize*(size-center)+center*4; i < (buffer->height * buffer->width * 4)-imgLineSize*(size-center)-center*4; i++)
////  {
////    value = 0;
////    for(y=0; y  < size; y++)
////    {
////      yOff = imgLineSize *( y-center);
////      for(x=0; x < size ; x++)
////      {
////        xOff = 4 * (x - center);
////        buffer->
////        value += matrix[y*size+x] * (buffer->bits[i+xOff+yOff]);
////      }
////    }
////    buffer->bits[i] = value;
////  }
////  //free memory and save the image
////  free(matrix);
////  return true;
////}
//
////
////  for (int h = 0; h < buffer->height; h++) {
////    for (int w = 0; w < buffer->width; w++) {
////      ((uint32_t*)buffer->bits)[ (h*buffer->stride) + w] &= 0xff00ff00;
////    }
//}

