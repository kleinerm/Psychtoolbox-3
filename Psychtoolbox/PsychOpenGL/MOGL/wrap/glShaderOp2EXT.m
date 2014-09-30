function glShaderOp2EXT( op, res, arg1, arg2 )

% glShaderOp2EXT  Interface to OpenGL function glShaderOp2EXT
%
% usage:  glShaderOp2EXT( op, res, arg1, arg2 )
%
% C function:  void glShaderOp2EXT(GLenum op, GLuint res, GLuint arg1, GLuint arg2)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glShaderOp2EXT', op, res, arg1, arg2 );

return
