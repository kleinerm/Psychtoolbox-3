function glMatrixMultfEXT( mode, m )

% glMatrixMultfEXT  Interface to OpenGL function glMatrixMultfEXT
%
% usage:  glMatrixMultfEXT( mode, m )
%
% C function:  void glMatrixMultfEXT(GLenum mode, const GLfloat* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixMultfEXT', mode, single(m) );

return
