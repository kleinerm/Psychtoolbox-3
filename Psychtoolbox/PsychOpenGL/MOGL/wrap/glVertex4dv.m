function glVertex4dv( v )

% glVertex4dv  Interface to OpenGL function glVertex4dv
%
% usage:  glVertex4dv( v )
%
% C function:  void glVertex4dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertex4dv', double(v) );

return
