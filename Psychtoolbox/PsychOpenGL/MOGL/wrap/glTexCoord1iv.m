function glTexCoord1iv( v )

% glTexCoord1iv  Interface to OpenGL function glTexCoord1iv
%
% usage:  glTexCoord1iv( v )
%
% C function:  void glTexCoord1iv(const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord1iv', int32(v) );

return
