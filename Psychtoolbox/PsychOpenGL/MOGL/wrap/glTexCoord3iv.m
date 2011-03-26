function glTexCoord3iv( v )

% glTexCoord3iv  Interface to OpenGL function glTexCoord3iv
%
% usage:  glTexCoord3iv( v )
%
% C function:  void glTexCoord3iv(const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord3iv', int32(v) );

return
