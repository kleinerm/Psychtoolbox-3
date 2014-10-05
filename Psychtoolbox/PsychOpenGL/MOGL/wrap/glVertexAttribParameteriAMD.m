function glVertexAttribParameteriAMD( index, pname, param )

% glVertexAttribParameteriAMD  Interface to OpenGL function glVertexAttribParameteriAMD
%
% usage:  glVertexAttribParameteriAMD( index, pname, param )
%
% C function:  void glVertexAttribParameteriAMD(GLuint index, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribParameteriAMD', index, pname, param );

return
