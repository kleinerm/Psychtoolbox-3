function glColor3iv( v )

% glColor3iv  Interface to OpenGL function glColor3iv
%
% usage:  glColor3iv( v )
%
% C function:  void glColor3iv(const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor3iv', int32(v) );

return
