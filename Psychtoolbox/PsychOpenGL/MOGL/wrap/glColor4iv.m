function glColor4iv( v )

% glColor4iv  Interface to OpenGL function glColor4iv
%
% usage:  glColor4iv( v )
%
% C function:  void glColor4iv(const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor4iv', int32(v) );

return
