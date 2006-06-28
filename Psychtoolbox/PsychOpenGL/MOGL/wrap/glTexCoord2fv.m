function glTexCoord2fv( v )

% glTexCoord2fv  Interface to OpenGL function glTexCoord2fv
%
% usage:  glTexCoord2fv( v )
%
% C function:  void glTexCoord2fv(const GLfloat* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2fv', moglsingle(v) );

return
