function glVertexAttribL4dEXT( index, x, y, z, w )

% glVertexAttribL4dEXT  Interface to OpenGL function glVertexAttribL4dEXT
%
% usage:  glVertexAttribL4dEXT( index, x, y, z, w )
%
% C function:  void glVertexAttribL4dEXT(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL4dEXT', index, x, y, z, w );

return
