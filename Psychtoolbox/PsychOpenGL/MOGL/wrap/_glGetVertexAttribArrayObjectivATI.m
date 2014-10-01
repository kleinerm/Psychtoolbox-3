function params = glGetVertexAttribArrayObjectivATI( index, pname )

% glGetVertexAttribArrayObjectivATI  Interface to OpenGL function glGetVertexAttribArrayObjectivATI
%
% usage:  params = glGetVertexAttribArrayObjectivATI( index, pname )
%
% C function:  void glGetVertexAttribArrayObjectivATI(GLuint index, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetVertexAttribArrayObjectivATI', index, pname, params );

return
