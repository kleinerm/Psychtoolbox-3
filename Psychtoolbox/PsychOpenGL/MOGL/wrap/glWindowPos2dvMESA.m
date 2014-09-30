function glWindowPos2dvMESA( v )

% glWindowPos2dvMESA  Interface to OpenGL function glWindowPos2dvMESA
%
% usage:  glWindowPos2dvMESA( v )
%
% C function:  void glWindowPos2dvMESA(const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2dvMESA', double(v) );

return
