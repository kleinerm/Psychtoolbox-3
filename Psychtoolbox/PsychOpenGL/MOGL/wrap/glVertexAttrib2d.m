function glVertexAttrib2d( index, x, y )

% glVertexAttrib2d  Interface to OpenGL function glVertexAttrib2d
%
% usage:  glVertexAttrib2d( index, x, y )
%
% C function:  void glVertexAttrib2d(GLuint index, GLdouble x, GLdouble y)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2d', index, x, y );

return
