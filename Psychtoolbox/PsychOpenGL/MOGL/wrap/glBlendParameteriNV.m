function glBlendParameteriNV( pname, value )

% glBlendParameteriNV  Interface to OpenGL function glBlendParameteriNV
%
% usage:  glBlendParameteriNV( pname, value )
%
% C function:  void glBlendParameteriNV(GLenum pname, GLint value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBlendParameteriNV', pname, value );

return
