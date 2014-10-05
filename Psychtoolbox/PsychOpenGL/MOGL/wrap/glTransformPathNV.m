function glTransformPathNV( resultPath, srcPath, transformType, transformValues )

% glTransformPathNV  Interface to OpenGL function glTransformPathNV
%
% usage:  glTransformPathNV( resultPath, srcPath, transformType, transformValues )
%
% C function:  void glTransformPathNV(GLuint resultPath, GLuint srcPath, GLenum transformType, const GLfloat* transformValues)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTransformPathNV', resultPath, srcPath, transformType, single(transformValues) );

return
