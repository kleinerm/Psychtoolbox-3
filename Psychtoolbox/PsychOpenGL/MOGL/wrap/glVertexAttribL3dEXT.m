function glVertexAttribL3dEXT( index, x, y, z )

% glVertexAttribL3dEXT  Interface to OpenGL function glVertexAttribL3dEXT
%
% usage:  glVertexAttribL3dEXT( index, x, y, z )
%
% C function:  void glVertexAttribL3dEXT(GLuint index, GLdouble x, GLdouble y, GLdouble z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL3dEXT', index, x, y, z );

return
