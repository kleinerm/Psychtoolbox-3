function glMatrixMultdEXT( mode, m )

% glMatrixMultdEXT  Interface to OpenGL function glMatrixMultdEXT
%
% usage:  glMatrixMultdEXT( mode, m )
%
% C function:  void glMatrixMultdEXT(GLenum mode, const GLdouble* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixMultdEXT', mode, double(m) );

return
