function glMatrixLoadTransposefEXT( mode, m )

% glMatrixLoadTransposefEXT  Interface to OpenGL function glMatrixLoadTransposefEXT
%
% usage:  glMatrixLoadTransposefEXT( mode, m )
%
% C function:  void glMatrixLoadTransposefEXT(GLenum mode, const GLfloat* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixLoadTransposefEXT', mode, single(m) );

return
