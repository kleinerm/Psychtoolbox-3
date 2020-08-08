function glRenderGpuMaskNV( mask )

% glRenderGpuMaskNV  Interface to OpenGL function glRenderGpuMaskNV
%
% usage:  glRenderGpuMaskNV( mask )
%
% C function:  void glRenderGpuMaskNV(GLbitfield mask)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glRenderGpuMaskNV', mask );

return
