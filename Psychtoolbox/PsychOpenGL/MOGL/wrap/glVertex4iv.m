function glVertex4iv( v )

% glVertex4iv  Interface to OpenGL function glVertex4iv
%
% usage:  glVertex4iv( v )
%
% C function:  void glVertex4iv(const GLint* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertex4iv', int32(v) );

return
