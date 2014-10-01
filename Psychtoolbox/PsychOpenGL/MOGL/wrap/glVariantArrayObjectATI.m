function glVariantArrayObjectATI( id, type, stride, buffer, offset )

% glVariantArrayObjectATI  Interface to OpenGL function glVariantArrayObjectATI
%
% usage:  glVariantArrayObjectATI( id, type, stride, buffer, offset )
%
% C function:  void glVariantArrayObjectATI(GLuint id, GLenum type, GLsizei stride, GLuint buffer, GLuint offset)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVariantArrayObjectATI', id, type, stride, buffer, offset );

return
