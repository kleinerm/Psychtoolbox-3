function glMulticastWaitSyncNV( signalGpu, waitGpuMask )

% glMulticastWaitSyncNV  Interface to OpenGL function glMulticastWaitSyncNV
%
% usage:  glMulticastWaitSyncNV( signalGpu, waitGpuMask )
%
% C function:  void glMulticastWaitSyncNV(GLuint signalGpu, GLbitfield waitGpuMask)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMulticastWaitSyncNV', signalGpu, waitGpuMask );

return
