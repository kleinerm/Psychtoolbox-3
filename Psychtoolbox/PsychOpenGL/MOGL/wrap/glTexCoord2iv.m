function glTexCoord2iv( v )

% glTexCoord2iv  Interface to OpenGL function glTexCoord2iv
%
% usage:  glTexCoord2iv( v )
%
% C function:  void glTexCoord2iv(const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2iv', int32(v) );

return
