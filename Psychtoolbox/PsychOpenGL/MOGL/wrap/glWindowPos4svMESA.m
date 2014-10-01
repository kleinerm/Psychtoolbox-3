function glWindowPos4svMESA( v )

% glWindowPos4svMESA  Interface to OpenGL function glWindowPos4svMESA
%
% usage:  glWindowPos4svMESA( v )
%
% C function:  void glWindowPos4svMESA(const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos4svMESA', int16(v) );

return
