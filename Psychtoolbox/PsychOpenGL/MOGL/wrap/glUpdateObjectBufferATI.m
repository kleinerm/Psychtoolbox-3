function glUpdateObjectBufferATI( buffer, offset, size, pointer, preserve )

% glUpdateObjectBufferATI  Interface to OpenGL function glUpdateObjectBufferATI
%
% usage:  glUpdateObjectBufferATI( buffer, offset, size, pointer, preserve )
%
% C function:  void glUpdateObjectBufferATI(GLuint buffer, GLuint offset, GLsizei size, const void* pointer, GLenum preserve)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glUpdateObjectBufferATI', buffer, offset, size, pointer, preserve );

return
