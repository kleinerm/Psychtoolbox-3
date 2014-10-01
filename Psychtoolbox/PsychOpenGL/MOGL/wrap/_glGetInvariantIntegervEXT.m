function data = glGetInvariantIntegervEXT( id, value )

% glGetInvariantIntegervEXT  Interface to OpenGL function glGetInvariantIntegervEXT
%
% usage:  data = glGetInvariantIntegervEXT( id, value )
%
% C function:  void glGetInvariantIntegervEXT(GLuint id, GLenum value, GLint* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = int32(0);

moglcore( 'glGetInvariantIntegervEXT', id, value, data );

return
