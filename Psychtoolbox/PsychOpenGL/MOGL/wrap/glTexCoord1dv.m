function glTexCoord1dv( v )

% glTexCoord1dv  Interface to OpenGL function glTexCoord1dv
%
% usage:  glTexCoord1dv( v )
%
% C function:  void glTexCoord1dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord1dv', double(v) );

return
