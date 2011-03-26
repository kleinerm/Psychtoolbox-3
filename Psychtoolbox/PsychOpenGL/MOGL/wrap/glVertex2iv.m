function glVertex2iv( v )

% glVertex2iv  Interface to OpenGL function glVertex2iv
%
% usage:  glVertex2iv( v )
%
% C function:  void glVertex2iv(const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertex2iv', int32(v) );

return
