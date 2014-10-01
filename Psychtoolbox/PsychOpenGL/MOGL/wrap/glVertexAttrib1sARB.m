function glVertexAttrib1sARB( index, x )

% glVertexAttrib1sARB  Interface to OpenGL function glVertexAttrib1sARB
%
% usage:  glVertexAttrib1sARB( index, x )
%
% C function:  void glVertexAttrib1sARB(GLuint index, GLshort x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1sARB', index, x );

return
