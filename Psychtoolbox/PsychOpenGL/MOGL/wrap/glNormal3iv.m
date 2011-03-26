function glNormal3iv( v )

% glNormal3iv  Interface to OpenGL function glNormal3iv
%
% usage:  glNormal3iv( v )
%
% C function:  void glNormal3iv(const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glNormal3iv', int32(v) );

return
