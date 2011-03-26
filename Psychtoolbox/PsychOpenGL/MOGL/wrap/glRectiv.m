function glRectiv( v1, v2 )

% glRectiv  Interface to OpenGL function glRectiv
%
% usage:  glRectiv( v1, v2 )
%
% C function:  void glRectiv(const GLint* v1, const GLint* v2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glRectiv', int32(v1), int32(v2) );

return
