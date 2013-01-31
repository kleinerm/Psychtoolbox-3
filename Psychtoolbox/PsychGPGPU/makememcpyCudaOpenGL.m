function makememcpyCudaOpenGL
% makememcpyCudaOpenGL - Build CUDA<->OpenGL interop helper mex file.
%
% This needs the NVidia CUDA-5.0 SDK or later installed.
%

if exist(['memcpyCudaOpenGL.' mexext], 'file');
    delete(['memcpyCudaOpenGL.' mexext]);
end

% -lcuda is optional at the moment. -framework CUDA is optional as well.
mex -v -g -largeArrayDims memcpyCudaOpenGL.c -I/usr/include/ -I"/Developer/NVIDIA/CUDA-5.0/include" CFLAGS="\$CFLAGS -F/System/Library/Frameworks" -L"/Developer/NVIDIA/CUDA-5.0/lib" -L/usr/local/cuda/lib LDFLAGS="\$LDFLAGS -Xlinker -rpath -Xlinker /usr/local/cuda/lib " -lcudart
delete *.o
