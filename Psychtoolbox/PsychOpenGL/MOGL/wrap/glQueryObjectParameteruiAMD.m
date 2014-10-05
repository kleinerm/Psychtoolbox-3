function glQueryObjectParameteruiAMD( target, id, pname, param )

% glQueryObjectParameteruiAMD  Interface to OpenGL function glQueryObjectParameteruiAMD
%
% usage:  glQueryObjectParameteruiAMD( target, id, pname, param )
%
% C function:  void glQueryObjectParameteruiAMD(GLenum target, GLuint id, GLenum pname, GLuint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glQueryObjectParameteruiAMD', target, id, pname, param );

return
