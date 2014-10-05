function params = glGetVertexAttribIiv( index, pname )

% glGetVertexAttribIiv  Interface to OpenGL function glGetVertexAttribIiv
%
% usage:  params = glGetVertexAttribIiv( index, pname )
%
% C function:  void glGetVertexAttribIiv(GLuint index, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetVertexAttribIiv', index, pname, params );

return
