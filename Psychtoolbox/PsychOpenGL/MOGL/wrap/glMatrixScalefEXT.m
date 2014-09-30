function glMatrixScalefEXT( mode, x, y, z )

% glMatrixScalefEXT  Interface to OpenGL function glMatrixScalefEXT
%
% usage:  glMatrixScalefEXT( mode, x, y, z )
%
% C function:  void glMatrixScalefEXT(GLenum mode, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMatrixScalefEXT', mode, x, y, z );

return
