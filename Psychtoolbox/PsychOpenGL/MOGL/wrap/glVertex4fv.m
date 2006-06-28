function glVertex4fv( v )

% glVertex4fv  Interface to OpenGL function glVertex4fv
%
% usage:  glVertex4fv( v )
%
% C function:  void glVertex4fv(const GLfloat* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertex4fv', moglsingle(v) );

return
