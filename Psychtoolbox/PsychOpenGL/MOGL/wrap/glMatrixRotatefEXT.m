function glMatrixRotatefEXT( mode, angle, x, y, z )

% glMatrixRotatefEXT  Interface to OpenGL function glMatrixRotatefEXT
%
% usage:  glMatrixRotatefEXT( mode, angle, x, y, z )
%
% C function:  void glMatrixRotatefEXT(GLenum mode, GLfloat angle, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMatrixRotatefEXT', mode, angle, x, y, z );

return
