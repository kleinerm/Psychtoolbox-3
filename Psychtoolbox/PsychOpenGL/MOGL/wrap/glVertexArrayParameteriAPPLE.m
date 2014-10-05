function glVertexArrayParameteriAPPLE( pname, param )

% glVertexArrayParameteriAPPLE  Interface to OpenGL function glVertexArrayParameteriAPPLE
%
% usage:  glVertexArrayParameteriAPPLE( pname, param )
%
% C function:  void glVertexArrayParameteriAPPLE(GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayParameteriAPPLE', pname, param );

return
