function glFinishFenceNV( fence )

% glFinishFenceNV  Interface to OpenGL function glFinishFenceNV
%
% usage:  glFinishFenceNV( fence )
%
% C function:  void glFinishFenceNV(GLuint fence)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glFinishFenceNV', fence );

return
