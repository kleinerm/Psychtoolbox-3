/*
 * memcpyCudaOpenGL.cpp -- Fast memory copy between CUDA and OpenGL resources.
 *
 * This function, compiled into a MEX file for Mathworks Matlab or GNU/Octave
 * allows to exchange data between NVidia CUDA memory buffers and other resources
 * and OpenGL resources, such as textures, renderbuffers, framebuffer objects,
 * pixelbuffer objects etc.
 *
 * See accompanying M-File for building the mex file.
 * See M-Wrapper file for actually using this low-level function.
 *
 * This file was originally developed for Psychtoolbox-3 (www.psychtoolbox.org)
 * but can be used for interop between any Matlab or Octave based CUDA and
 * OpenGL toolkit.
 *
 *
 * Copyright (c) 2013 Mario Kleiner. Licensed to you under the MIT license:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do so, subject to the
 * following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
 * NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* MATLAB / OCTAVE includes: */
#include "mex.h"

/* CUDA includes: */
#include "cuda_runtime_api.h"
#include "cuda_gl_interop.h"

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

  unsigned int glhandle, gltarget, direction, flags;
  unsigned long nrbytes;
  void *gpuptr;
  cudaError_t cudastatus = cudaSuccess;
  cudaGraphicsResource_t resource = NULL;
  struct cudaArray *mappedArray = NULL;
  static int firsttime = 1;
  
  if (firsttime) {
      firsttime = 0;
      mexPrintf("\nmemcpyCudaOpenGL: A simple CUDA <=> OpenGL interoperation interface.\n");
      mexPrintf("(c) 2013 by Mario Kleiner. Licensed to you under the MIT license.\n\n");
  }
  
  if (nrhs!=5) mexErrMsgTxt("Wrong number of arguments");

  /* Retrieve OpenGL object handle to our image buffer: */
  glhandle = (unsigned int) mxGetScalar(prhs[0]);

  /* Get GLEnum target: */
  gltarget = (unsigned int) mxGetScalar(prhs[1]);
  
  /* Retrieve CUDA memory pointer to source/destination CUDA memory buffer: */
  gpuptr = (void*) (unsigned long) mxGetScalar(prhs[2]);
  
  /* Retrieve number of bytes to copy: */
  nrbytes = (unsigned long) mxGetScalar(prhs[3]);
  
  /* Retrieve direction: 0 = OpenGL -> CUDA, 1 = CUDA -> OpenGL : */
  direction = (unsigned int) mxGetScalar(prhs[4]);

  /* Define CUDA optimization flags, depending if this is a OpenGL->CUDA or
   * CUDA->OpenGL copy operation.
   */
  flags = (direction) ? cudaGraphicsRegisterFlagsWriteDiscard : cudaGraphicsRegisterFlagsReadOnly;
  
  /* Register OpenGL object with CUDA as 'resource': */
  cudastatus = cudaGraphicsGLRegisterImage(&resource, glhandle, gltarget, flags);
  if (cudastatus != cudaSuccess) {
    mexPrintf("\nmemcpyCudaOpenGL: ERROR in cudaGraphicsGLRegisterImage(): %s\n", cudaGetErrorString(cudastatus));
    goto err_final;
  }

  /* Map the 'resource': */
  cudastatus = cudaGraphicsMapResources(1, &resource, 0);
  if (cudastatus != cudaSuccess) {
    mexPrintf("\nmemcpyCudaOpenGL: ERROR in cudaGraphicsMapResources(): %s\n", cudaGetErrorString(cudastatus));
    goto err_unregister;
  }
  
  /* Get mapped resource image array handle: */
  cudastatus = cudaGraphicsSubResourceGetMappedArray(&mappedArray, resource, 0, 0);
  if (cudastatus != cudaSuccess) {
    mexPrintf("\nmemcpyCudaOpenGL: ERROR in cudaGraphicsSubResourceGetMappedArray(): %s\n", cudaGetErrorString(cudastatus));
    goto err_unmap;
  }
  
  /* Copy from OpenGL object to CUDA buffer? */
  if (direction == 0) {
      /* OpenGL -> CUDA copy: */
      cudastatus = cudaMemcpyFromArrayAsync(gpuptr, (const struct cudaArray*) mappedArray, 0, 0, (size_t) nrbytes, cudaMemcpyDeviceToDevice, 0);
      if (cudastatus != cudaSuccess) {
          mexPrintf("\nmemcpyCudaOpenGL: ERROR in cudaMemcpyFromArray(): %s\n", cudaGetErrorString(cudastatus));
          goto err_unmap;
      }
  }

  if (direction == 1) {
      /* CUDA -> OpenGL copy: */
      cudastatus = cudaMemcpyToArrayAsync((struct cudaArray*) mappedArray, 0, 0, (const void*) gpuptr, (size_t) nrbytes, cudaMemcpyDeviceToDevice, 0);
      if (cudastatus != cudaSuccess) {
          mexPrintf("\nmemcpyCudaOpenGL: ERROR in cudaMemcpyToArray(): %s\n", cudaGetErrorString(cudastatus));
          goto err_unmap;
      }
  }

err_unmap:
  
  /* Unmap the 'resource': */
  cudastatus = cudaGraphicsUnmapResources(1, &resource, 0);
  if (cudastatus != cudaSuccess) {
    mexPrintf("\nmemcpyCudaOpenGL: ERROR in cudaGraphicsUnmapResources(): %s\n", cudaGetErrorString(cudastatus));
  }

err_unregister:
  
  /* Unregister the 'resource': */
  cudastatus = cudaGraphicsUnregisterResource(resource);
  if (cudastatus != cudaSuccess) {
    mexPrintf("\nmemcpyCudaOpenGL: ERROR in cudaGraphicsUnregisterResource(): %s\n", cudaGetErrorString(cudastatus));
  }

err_final:      
      
  if (cudastatus != cudaSuccess) mexErrMsgTxt("Error in memcpyCudaOpenGL(), reason see above.");
}

