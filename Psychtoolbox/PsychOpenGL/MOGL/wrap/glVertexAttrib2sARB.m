function glVertexAttrib2sARB( index, x, y )

% glVertexAttrib2sARB  Interface to OpenGL function glVertexAttrib2sARB
%
% usage:  glVertexAttrib2sARB( index, x, y )
%
% C function:  void glVertexAttrib2sARB(GLuint index, GLshort x, GLshort y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2sARB', index, x, y );

return
