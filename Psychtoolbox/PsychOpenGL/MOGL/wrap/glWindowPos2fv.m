function glWindowPos2fv( v )

% glWindowPos2fv  Interface to OpenGL function glWindowPos2fv
%
% usage:  glWindowPos2fv( v )
%
% C function:  void glWindowPos2fv(const GLfloat* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2fv', single(v) );

return
