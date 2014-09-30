function glWindowPos2iMESA( x, y )

% glWindowPos2iMESA  Interface to OpenGL function glWindowPos2iMESA
%
% usage:  glWindowPos2iMESA( x, y )
%
% C function:  void glWindowPos2iMESA(GLint x, GLint y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2iMESA', x, y );

return
