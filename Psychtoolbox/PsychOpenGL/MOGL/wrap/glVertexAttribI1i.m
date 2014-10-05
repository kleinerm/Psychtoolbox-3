function glVertexAttribI1i( index, x )

% glVertexAttribI1i  Interface to OpenGL function glVertexAttribI1i
%
% usage:  glVertexAttribI1i( index, x )
%
% C function:  void glVertexAttribI1i(GLuint index, GLint x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI1i', index, x );

return
