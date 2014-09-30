function glWindowPos2fvMESA( v )

% glWindowPos2fvMESA  Interface to OpenGL function glWindowPos2fvMESA
%
% usage:  glWindowPos2fvMESA( v )
%
% C function:  void glWindowPos2fvMESA(const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2fvMESA', single(v) );

return
