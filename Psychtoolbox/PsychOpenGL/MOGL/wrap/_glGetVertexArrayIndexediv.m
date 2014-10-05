function param = glGetVertexArrayIndexediv( vaobj, index, pname )

% glGetVertexArrayIndexediv  Interface to OpenGL function glGetVertexArrayIndexediv
%
% usage:  param = glGetVertexArrayIndexediv( vaobj, index, pname )
%
% C function:  void glGetVertexArrayIndexediv(GLuint vaobj, GLuint index, GLenum pname, GLint* param)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

param = int32(0);

moglcore( 'glGetVertexArrayIndexediv', vaobj, index, pname, param );

return
