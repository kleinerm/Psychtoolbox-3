function glWindowPos4ivMESA( v )

% glWindowPos4ivMESA  Interface to OpenGL function glWindowPos4ivMESA
%
% usage:  glWindowPos4ivMESA( v )
%
% C function:  void glWindowPos4ivMESA(const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos4ivMESA', int32(v) );

return
