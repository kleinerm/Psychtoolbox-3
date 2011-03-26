function glVertex2dv( v )

% glVertex2dv  Interface to OpenGL function glVertex2dv
%
% usage:  glVertex2dv( v )
%
% C function:  void glVertex2dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertex2dv', double(v) );

return
