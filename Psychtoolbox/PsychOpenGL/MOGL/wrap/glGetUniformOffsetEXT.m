function r = glGetUniformOffsetEXT( program, location )

% glGetUniformOffsetEXT  Interface to OpenGL function glGetUniformOffsetEXT
%
% usage:  r = glGetUniformOffsetEXT( program, location )
%
% C function:  GLintptr glGetUniformOffsetEXT(GLuint program, GLint location)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetUniformOffsetEXT', program, location );

return
