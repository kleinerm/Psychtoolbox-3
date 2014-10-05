function param = glGetVertexArrayIndexed64iv( vaobj, index, pname )

% glGetVertexArrayIndexed64iv  Interface to OpenGL function glGetVertexArrayIndexed64iv
%
% usage:  param = glGetVertexArrayIndexed64iv( vaobj, index, pname )
%
% C function:  void glGetVertexArrayIndexed64iv(GLuint vaobj, GLuint index, GLenum pname, GLint64* param)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

param = int64(0);

moglcore( 'glGetVertexArrayIndexed64iv', vaobj, index, pname, param );

return
