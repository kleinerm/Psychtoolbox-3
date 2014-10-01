function glWindowPos3dvMESA( v )

% glWindowPos3dvMESA  Interface to OpenGL function glWindowPos3dvMESA
%
% usage:  glWindowPos3dvMESA( v )
%
% C function:  void glWindowPos3dvMESA(const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3dvMESA', double(v) );

return
