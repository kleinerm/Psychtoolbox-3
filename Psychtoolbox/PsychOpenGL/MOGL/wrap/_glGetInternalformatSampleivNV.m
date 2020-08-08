function params = glGetInternalformatSampleivNV( target, internalformat, samples, pname, bufSize )

% glGetInternalformatSampleivNV  Interface to OpenGL function glGetInternalformatSampleivNV
%
% usage:  params = glGetInternalformatSampleivNV( target, internalformat, samples, pname, bufSize )
%
% C function:  void glGetInternalformatSampleivNV(GLenum target, GLenum internalformat, GLsizei samples, GLenum pname, GLsizei bufSize, GLint* params)

% 08-Aug-2020 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetInternalformatSampleivNV', target, internalformat, samples, pname, bufSize, params );

return
