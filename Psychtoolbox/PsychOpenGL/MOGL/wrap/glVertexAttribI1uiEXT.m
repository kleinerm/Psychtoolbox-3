function glVertexAttribI1uiEXT( index, x )

% glVertexAttribI1uiEXT  Interface to OpenGL function glVertexAttribI1uiEXT
%
% usage:  glVertexAttribI1uiEXT( index, x )
%
% C function:  void glVertexAttribI1uiEXT(GLuint index, GLuint x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI1uiEXT', index, x );

return
