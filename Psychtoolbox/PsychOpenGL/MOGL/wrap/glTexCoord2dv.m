function glTexCoord2dv( v )

% glTexCoord2dv  Interface to OpenGL function glTexCoord2dv
%
% usage:  glTexCoord2dv( v )
%
% C function:  void glTexCoord2dv(const GLdouble* v)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glTexCoord2dv', double(v) );

return
