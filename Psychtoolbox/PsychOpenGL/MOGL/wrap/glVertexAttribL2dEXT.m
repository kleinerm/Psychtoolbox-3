function glVertexAttribL2dEXT( index, x, y )

% glVertexAttribL2dEXT  Interface to OpenGL function glVertexAttribL2dEXT
%
% usage:  glVertexAttribL2dEXT( index, x, y )
%
% C function:  void glVertexAttribL2dEXT(GLuint index, GLdouble x, GLdouble y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL2dEXT', index, x, y );

return
