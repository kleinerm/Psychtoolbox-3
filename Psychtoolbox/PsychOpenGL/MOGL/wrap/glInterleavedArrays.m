function glInterleavedArrays( format, stride, pointer )

% glInterleavedArrays  Interface to OpenGL function glInterleavedArrays
%
% usage:  glInterleavedArrays( format, stride, pointer )
%
% C function:  void glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid* pointer)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glInterleavedArrays', format, stride, pointer );

return
