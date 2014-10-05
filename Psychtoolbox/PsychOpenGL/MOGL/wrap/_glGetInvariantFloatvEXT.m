function data = glGetInvariantFloatvEXT( id, value )

% glGetInvariantFloatvEXT  Interface to OpenGL function glGetInvariantFloatvEXT
%
% usage:  data = glGetInvariantFloatvEXT( id, value )
%
% C function:  void glGetInvariantFloatvEXT(GLuint id, GLenum value, GLfloat* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = single(0);

moglcore( 'glGetInvariantFloatvEXT', id, value, data );

return
