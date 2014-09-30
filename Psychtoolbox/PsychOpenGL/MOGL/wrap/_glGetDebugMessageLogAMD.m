function [ r, categories, severities, ids, lengths, message ] = glGetDebugMessageLogAMD( count, bufsize )

% glGetDebugMessageLogAMD  Interface to OpenGL function glGetDebugMessageLogAMD
%
% usage:  [ r, categories, severities, ids, lengths, message ] = glGetDebugMessageLogAMD( count, bufsize )
%
% C function:  GLuint glGetDebugMessageLogAMD(GLuint count, GLsizei bufsize, GLenum* categories, GLuint* severities, GLuint* ids, GLsizei* lengths, GLchar* message)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

categories = uint32(0);
severities = uint32(0);
ids = uint32(0);
lengths = int32(0);
message = uint8(0);

r = moglcore( 'glGetDebugMessageLogAMD', count, bufsize, categories, severities, ids, lengths, message );

return
