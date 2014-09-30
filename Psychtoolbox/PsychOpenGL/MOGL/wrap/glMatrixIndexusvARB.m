function glMatrixIndexusvARB( size, indices )

% glMatrixIndexusvARB  Interface to OpenGL function glMatrixIndexusvARB
%
% usage:  glMatrixIndexusvARB( size, indices )
%
% C function:  void glMatrixIndexusvARB(GLint size, const GLushort* indices)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixIndexusvARB', size, uint16(indices) );

return
