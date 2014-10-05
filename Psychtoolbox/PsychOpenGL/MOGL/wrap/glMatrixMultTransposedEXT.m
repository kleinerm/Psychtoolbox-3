function glMatrixMultTransposedEXT( mode, m )

% glMatrixMultTransposedEXT  Interface to OpenGL function glMatrixMultTransposedEXT
%
% usage:  glMatrixMultTransposedEXT( mode, m )
%
% C function:  void glMatrixMultTransposedEXT(GLenum mode, const GLdouble* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixMultTransposedEXT', mode, double(m) );

return
