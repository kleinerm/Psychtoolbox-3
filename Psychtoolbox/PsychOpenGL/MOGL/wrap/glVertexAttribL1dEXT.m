function glVertexAttribL1dEXT( index, x )

% glVertexAttribL1dEXT  Interface to OpenGL function glVertexAttribL1dEXT
%
% usage:  glVertexAttribL1dEXT( index, x )
%
% C function:  void glVertexAttribL1dEXT(GLuint index, GLdouble x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL1dEXT', index, x );

return
