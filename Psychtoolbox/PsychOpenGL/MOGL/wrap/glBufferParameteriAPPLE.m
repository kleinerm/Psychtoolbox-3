function glBufferParameteriAPPLE( target, pname, param )

% glBufferParameteriAPPLE  Interface to OpenGL function glBufferParameteriAPPLE
%
% usage:  glBufferParameteriAPPLE( target, pname, param )
%
% C function:  void glBufferParameteriAPPLE(GLenum target, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBufferParameteriAPPLE', target, pname, param );

return
