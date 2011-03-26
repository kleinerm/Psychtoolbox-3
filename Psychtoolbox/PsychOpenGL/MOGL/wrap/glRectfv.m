function glRectfv( v1, v2 )

% glRectfv  Interface to OpenGL function glRectfv
%
% usage:  glRectfv( v1, v2 )
%
% C function:  void glRectfv(const GLfloat* v1, const GLfloat* v2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glRectfv', single(v1), single(v2) );

return
