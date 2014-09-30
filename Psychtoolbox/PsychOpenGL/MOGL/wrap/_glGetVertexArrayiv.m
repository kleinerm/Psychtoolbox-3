function param = glGetVertexArrayiv( vaobj, pname )

% glGetVertexArrayiv  Interface to OpenGL function glGetVertexArrayiv
%
% usage:  param = glGetVertexArrayiv( vaobj, pname )
%
% C function:  void glGetVertexArrayiv(GLuint vaobj, GLenum pname, GLint* param)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

param = int32(0);

moglcore( 'glGetVertexArrayiv', vaobj, pname, param );

return
