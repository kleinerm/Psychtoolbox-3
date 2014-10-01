function glMapNamedBuffer( buffer, access )

% glMapNamedBuffer  Interface to OpenGL function glMapNamedBuffer
%
% usage:  glMapNamedBuffer( buffer, access )
%
% C function:  void* glMapNamedBuffer(GLuint buffer, GLenum access)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMapNamedBuffer', buffer, access );

return
