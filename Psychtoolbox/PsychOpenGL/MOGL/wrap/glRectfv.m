function glRectfv( v1, v2 )

% glRectfv  Interface to OpenGL function glRectfv
%
% usage:  glRectfv( v1, v2 )
%
% C function:  void glRectfv(const GLfloat* v1, const GLfloat* v2)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glRectfv', moglsingle(v1), moglsingle(v2) );

return
