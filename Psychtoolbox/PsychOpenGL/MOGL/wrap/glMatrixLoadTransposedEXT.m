function glMatrixLoadTransposedEXT( mode, m )

% glMatrixLoadTransposedEXT  Interface to OpenGL function glMatrixLoadTransposedEXT
%
% usage:  glMatrixLoadTransposedEXT( mode, m )
%
% C function:  void glMatrixLoadTransposedEXT(GLenum mode, const GLdouble* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixLoadTransposedEXT', mode, double(m) );

return
