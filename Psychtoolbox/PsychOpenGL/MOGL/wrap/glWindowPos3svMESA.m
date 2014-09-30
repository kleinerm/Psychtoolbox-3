function glWindowPos3svMESA( v )

% glWindowPos3svMESA  Interface to OpenGL function glWindowPos3svMESA
%
% usage:  glWindowPos3svMESA( v )
%
% C function:  void glWindowPos3svMESA(const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3svMESA', int16(v) );

return
