function glMatrixTranslatefEXT( mode, x, y, z )

% glMatrixTranslatefEXT  Interface to OpenGL function glMatrixTranslatefEXT
%
% usage:  glMatrixTranslatefEXT( mode, x, y, z )
%
% C function:  void glMatrixTranslatefEXT(GLenum mode, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMatrixTranslatefEXT', mode, x, y, z );

return
