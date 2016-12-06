function [ r, sources, types, ids, severities, lengths, messageLog ] = glGetDebugMessageLogARB( count, bufsize )

% glGetDebugMessageLogARB  Interface to OpenGL function glGetDebugMessageLogARB
%
% usage:  [ r, sources, types, ids, severities, lengths, messageLog ] = glGetDebugMessageLogARB( count, bufsize )
%
% C function:  GLuint glGetDebugMessageLogARB(GLuint count, GLsizei bufsize, GLenum* sources, GLenum* types, GLuint* ids, GLenum* severities, GLsizei* lengths, GLchar* messageLog)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

count = count + 1;
sources = uint32(zeros(1,count));
types = uint32(zeros(1,count));
ids = uint32(zeros(1,count));
severities = uint32(zeros(1,count));
lengths = int32(zeros(1,count));
messageLog = uint8(zeros(1,bufsize));

r = moglcore( 'glGetDebugMessageLogARB', count, bufsize, sources, types, ids, severities, lengths, messageLog );
messageLog = char(messageLog);
sources = sources(1:end-1);
types = types(1:end-1);
ids = ids(1:end-1);
severities = severities(1:end-1);
lengths = lengths(1:end-1);
return
