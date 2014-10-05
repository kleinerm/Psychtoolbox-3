function params = glGetVertexAttribIuiv( index, pname )

% glGetVertexAttribIuiv  Interface to OpenGL function glGetVertexAttribIuiv
%
% usage:  params = glGetVertexAttribIuiv( index, pname )
%
% C function:  void glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetVertexAttribIuiv', index, pname, params );

return
