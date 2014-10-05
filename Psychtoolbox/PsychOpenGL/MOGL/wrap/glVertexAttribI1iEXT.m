function glVertexAttribI1iEXT( index, x )

% glVertexAttribI1iEXT  Interface to OpenGL function glVertexAttribI1iEXT
%
% usage:  glVertexAttribI1iEXT( index, x )
%
% C function:  void glVertexAttribI1iEXT(GLuint index, GLint x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI1iEXT', index, x );

return
