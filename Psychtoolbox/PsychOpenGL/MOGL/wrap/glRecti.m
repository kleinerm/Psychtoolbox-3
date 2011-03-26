function glRecti( x1, y1, x2, y2 )

% glRecti  Interface to OpenGL function glRecti
%
% usage:  glRecti( x1, y1, x2, y2 )
%
% C function:  void glRecti(GLint x1, GLint y1, GLint x2, GLint y2)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glRecti', x1, y1, x2, y2 );

return
