function glWindowPos2dMESA( x, y )

% glWindowPos2dMESA  Interface to OpenGL function glWindowPos2dMESA
%
% usage:  glWindowPos2dMESA( x, y )
%
% C function:  void glWindowPos2dMESA(GLdouble x, GLdouble y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2dMESA', x, y );

return
