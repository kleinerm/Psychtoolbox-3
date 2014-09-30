function glMatrixLoaddEXT( mode, m )

% glMatrixLoaddEXT  Interface to OpenGL function glMatrixLoaddEXT
%
% usage:  glMatrixLoaddEXT( mode, m )
%
% C function:  void glMatrixLoaddEXT(GLenum mode, const GLdouble* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixLoaddEXT', mode, double(m) );

return
