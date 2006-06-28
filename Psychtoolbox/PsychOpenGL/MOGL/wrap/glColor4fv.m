function glColor4fv( v )

% glColor4fv  Interface to OpenGL function glColor4fv
%
% usage:  glColor4fv( v )
%
% C function:  void glColor4fv(const GLfloat* v)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor4fv', moglsingle(v) );

return
