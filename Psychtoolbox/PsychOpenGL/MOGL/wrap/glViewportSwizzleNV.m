function glViewportSwizzleNV( index, swizzlex, swizzley, swizzlez, swizzlew )

% glViewportSwizzleNV  Interface to OpenGL function glViewportSwizzleNV
%
% usage:  glViewportSwizzleNV( index, swizzlex, swizzley, swizzlez, swizzlew )
%
% C function:  void glViewportSwizzleNV(GLuint index, GLenum swizzlex, GLenum swizzley, GLenum swizzlez, GLenum swizzlew)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glViewportSwizzleNV', index, swizzlex, swizzley, swizzlez, swizzlew );

return
