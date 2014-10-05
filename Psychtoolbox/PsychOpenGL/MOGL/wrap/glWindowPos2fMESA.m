function glWindowPos2fMESA( x, y )

% glWindowPos2fMESA  Interface to OpenGL function glWindowPos2fMESA
%
% usage:  glWindowPos2fMESA( x, y )
%
% C function:  void glWindowPos2fMESA(GLfloat x, GLfloat y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2fMESA', x, y );

return
