function params = glGetVertexAttribArrayObjectfvATI( index, pname )

% glGetVertexAttribArrayObjectfvATI  Interface to OpenGL function glGetVertexAttribArrayObjectfvATI
%
% usage:  params = glGetVertexAttribArrayObjectfvATI( index, pname )
%
% C function:  void glGetVertexAttribArrayObjectfvATI(GLuint index, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetVertexAttribArrayObjectfvATI', index, pname, params );

return
