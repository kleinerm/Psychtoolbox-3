function glWindowPos3fvMESA( v )

% glWindowPos3fvMESA  Interface to OpenGL function glWindowPos3fvMESA
%
% usage:  glWindowPos3fvMESA( v )
%
% C function:  void glWindowPos3fvMESA(const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3fvMESA', single(v) );

return
