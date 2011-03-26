function glVertexAttrib1d( index, x )

% glVertexAttrib1d  Interface to OpenGL function glVertexAttrib1d
%
% usage:  glVertexAttrib1d( index, x )
%
% C function:  void glVertexAttrib1d(GLuint index, GLdouble x)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1d', index, x );

return
