function glMatrixIndexuivARB( size, indices )

% glMatrixIndexuivARB  Interface to OpenGL function glMatrixIndexuivARB
%
% usage:  glMatrixIndexuivARB( size, indices )
%
% C function:  void glMatrixIndexuivARB(GLint size, const GLuint* indices)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixIndexuivARB', size, uint32(indices) );

return
