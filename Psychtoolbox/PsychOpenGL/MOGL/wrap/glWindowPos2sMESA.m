function glWindowPos2sMESA( x, y )

% glWindowPos2sMESA  Interface to OpenGL function glWindowPos2sMESA
%
% usage:  glWindowPos2sMESA( x, y )
%
% C function:  void glWindowPos2sMESA(GLshort x, GLshort y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2sMESA', x, y );

return
