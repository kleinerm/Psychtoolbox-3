function glColor4fv( v )

% glColor4fv  Interface to OpenGL function glColor4fv
%
% usage:  glColor4fv( v )
%
% C function:  void glColor4fv(const GLfloat* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glColor4fv', single(v) );

return
