function glSetFenceNV( fence, condition )

% glSetFenceNV  Interface to OpenGL function glSetFenceNV
%
% usage:  glSetFenceNV( fence, condition )
%
% C function:  void glSetFenceNV(GLuint fence, GLenum condition)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glSetFenceNV', fence, condition );

return
