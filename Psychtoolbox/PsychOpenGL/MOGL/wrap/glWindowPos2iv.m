function glWindowPos2iv( v )

% glWindowPos2iv  Interface to OpenGL function glWindowPos2iv
%
% usage:  glWindowPos2iv( v )
%
% C function:  void glWindowPos2iv(const GLint* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2iv', int32(v) );

return
