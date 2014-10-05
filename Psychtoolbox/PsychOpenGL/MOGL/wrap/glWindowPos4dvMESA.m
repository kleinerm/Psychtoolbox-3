function glWindowPos4dvMESA( v )

% glWindowPos4dvMESA  Interface to OpenGL function glWindowPos4dvMESA
%
% usage:  glWindowPos4dvMESA( v )
%
% C function:  void glWindowPos4dvMESA(const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos4dvMESA', double(v) );

return
