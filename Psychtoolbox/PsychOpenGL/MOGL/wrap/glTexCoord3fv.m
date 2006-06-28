function glTexCoord3fv( v )

% glTexCoord3fv  Interface to OpenGL function glTexCoord3fv
%
% usage:  glTexCoord3fv( v )
%
% C function:  void glTexCoord3fv(const GLfloat* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord3fv', moglsingle(v) );

return
