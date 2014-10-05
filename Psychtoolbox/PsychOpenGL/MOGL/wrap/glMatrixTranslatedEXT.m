function glMatrixTranslatedEXT( mode, x, y, z )

% glMatrixTranslatedEXT  Interface to OpenGL function glMatrixTranslatedEXT
%
% usage:  glMatrixTranslatedEXT( mode, x, y, z )
%
% C function:  void glMatrixTranslatedEXT(GLenum mode, GLdouble x, GLdouble y, GLdouble z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glMatrixTranslatedEXT', mode, x, y, z );

return
