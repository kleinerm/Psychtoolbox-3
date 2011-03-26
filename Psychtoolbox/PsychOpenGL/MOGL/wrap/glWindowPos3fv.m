function glWindowPos3fv( v )

% glWindowPos3fv  Interface to OpenGL function glWindowPos3fv
%
% usage:  glWindowPos3fv( v )
%
% C function:  void glWindowPos3fv(const GLfloat* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos3fv', single(v) );

return
