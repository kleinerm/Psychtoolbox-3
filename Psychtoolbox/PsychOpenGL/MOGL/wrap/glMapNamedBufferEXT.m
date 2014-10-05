function glMapNamedBufferEXT( buffer, access )

% glMapNamedBufferEXT  Interface to OpenGL function glMapNamedBufferEXT
%
% usage:  glMapNamedBufferEXT( buffer, access )
%
% C function:  void* glMapNamedBufferEXT(GLuint buffer, GLenum access)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMapNamedBufferEXT', buffer, access );

return
