function glRectdv( v1, v2 )

% glRectdv  Interface to OpenGL function glRectdv
%
% usage:  glRectdv( v1, v2 )
%
% C function:  void glRectdv(const GLdouble* v1, const GLdouble* v2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glRectdv', double(v1), double(v2) );

return
