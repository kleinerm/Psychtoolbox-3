function glMatrixMultTransposefEXT( mode, m )

% glMatrixMultTransposefEXT  Interface to OpenGL function glMatrixMultTransposefEXT
%
% usage:  glMatrixMultTransposefEXT( mode, m )
%
% C function:  void glMatrixMultTransposefEXT(GLenum mode, const GLfloat* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixMultTransposefEXT', mode, single(m) );

return
