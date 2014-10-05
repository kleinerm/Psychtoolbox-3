function glNamedBufferData( buffer, size, data, usage )

% glNamedBufferData  Interface to OpenGL function glNamedBufferData
%
% usage:  glNamedBufferData( buffer, size, data, usage )
%
% C function:  void glNamedBufferData(GLuint buffer, GLsizei size, const void* data, GLenum usage)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glNamedBufferData', buffer, size, data, usage );

return
