function data = glGetLocalConstantFloatvEXT( id, value )

% glGetLocalConstantFloatvEXT  Interface to OpenGL function glGetLocalConstantFloatvEXT
%
% usage:  data = glGetLocalConstantFloatvEXT( id, value )
%
% C function:  void glGetLocalConstantFloatvEXT(GLuint id, GLenum value, GLfloat* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

data = single(0);

moglcore( 'glGetLocalConstantFloatvEXT', id, value, data );

return
