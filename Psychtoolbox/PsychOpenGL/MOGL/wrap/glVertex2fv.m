function glVertex2fv( v )

% glVertex2fv  Interface to OpenGL function glVertex2fv
%
% usage:  glVertex2fv( v )
%
% C function:  void glVertex2fv(const GLfloat* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertex2fv', moglsingle(v) );

return
