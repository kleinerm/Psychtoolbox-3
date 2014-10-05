function data = glGetInvariantBooleanvEXT( id, value )

% glGetInvariantBooleanvEXT  Interface to OpenGL function glGetInvariantBooleanvEXT
%
% usage:  data = glGetInvariantBooleanvEXT( id, value )
%
% C function:  void glGetInvariantBooleanvEXT(GLuint id, GLenum value, GLboolean* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = uint8(0);

moglcore( 'glGetInvariantBooleanvEXT', id, value, data );

return
