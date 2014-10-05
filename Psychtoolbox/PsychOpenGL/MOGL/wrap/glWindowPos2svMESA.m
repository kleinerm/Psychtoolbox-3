function glWindowPos2svMESA( v )

% glWindowPos2svMESA  Interface to OpenGL function glWindowPos2svMESA
%
% usage:  glWindowPos2svMESA( v )
%
% C function:  void glWindowPos2svMESA(const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2svMESA', int16(v) );

return
