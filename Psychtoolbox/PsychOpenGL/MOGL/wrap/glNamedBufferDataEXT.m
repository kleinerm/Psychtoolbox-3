function glNamedBufferDataEXT( buffer, size, data, usage )

% glNamedBufferDataEXT  Interface to OpenGL function glNamedBufferDataEXT
%
% usage:  glNamedBufferDataEXT( buffer, size, data, usage )
%
% C function:  void glNamedBufferDataEXT(GLuint buffer, GLsizeiptr size, const void* data, GLenum usage)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedBufferDataEXT', buffer, size, data, usage );

return
