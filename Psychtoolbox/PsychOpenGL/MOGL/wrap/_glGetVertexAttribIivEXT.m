function params = glGetVertexAttribIivEXT( index, pname )

% glGetVertexAttribIivEXT  Interface to OpenGL function glGetVertexAttribIivEXT
%
% usage:  params = glGetVertexAttribIivEXT( index, pname )
%
% C function:  void glGetVertexAttribIivEXT(GLuint index, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetVertexAttribIivEXT', index, pname, params );

return
