function glSwizzleEXT( res, in, outX, outY, outZ, outW )

% glSwizzleEXT  Interface to OpenGL function glSwizzleEXT
%
% usage:  glSwizzleEXT( res, in, outX, outY, outZ, outW )
%
% C function:  void glSwizzleEXT(GLuint res, GLuint in, GLenum outX, GLenum outY, GLenum outZ, GLenum outW)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glSwizzleEXT', res, in, outX, outY, outZ, outW );

return
