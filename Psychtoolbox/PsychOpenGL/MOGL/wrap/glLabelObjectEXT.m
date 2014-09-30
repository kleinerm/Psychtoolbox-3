function glLabelObjectEXT( type, object, length, label )

% glLabelObjectEXT  Interface to OpenGL function glLabelObjectEXT
%
% usage:  glLabelObjectEXT( type, object, length, label )
%
% C function:  void glLabelObjectEXT(GLenum type, GLuint object, GLsizei length, const GLchar* label)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glLabelObjectEXT', type, object, length, uint8(label) );

return
