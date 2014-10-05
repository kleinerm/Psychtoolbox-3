function params = glGetVertexAttribIuivEXT( index, pname )

% glGetVertexAttribIuivEXT  Interface to OpenGL function glGetVertexAttribIuivEXT
%
% usage:  params = glGetVertexAttribIuivEXT( index, pname )
%
% C function:  void glGetVertexAttribIuivEXT(GLuint index, GLenum pname, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetVertexAttribIuivEXT', index, pname, params );

return
