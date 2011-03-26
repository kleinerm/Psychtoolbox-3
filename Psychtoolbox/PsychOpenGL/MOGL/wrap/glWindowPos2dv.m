function glWindowPos2dv( v )

% glWindowPos2dv  Interface to OpenGL function glWindowPos2dv
%
% usage:  glWindowPos2dv( v )
%
% C function:  void glWindowPos2dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glWindowPos2dv', double(v) );

return
