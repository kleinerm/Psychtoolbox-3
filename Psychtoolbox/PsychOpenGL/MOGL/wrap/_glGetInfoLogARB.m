function [ length, infoLog ] = glGetInfoLogARB( obj, maxLength )

% glGetInfoLogARB  Interface to OpenGL function glGetInfoLogARB
%
% usage:  [ length, infoLog ] = glGetInfoLogARB( obj, maxLength )
%
% C function:  void glGetInfoLogARB(GLhandleARB obj, GLsizei maxLength, GLsizei* length, GLcharARB* infoLog)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

length = int32(0);
infoLog = uint8(0);

moglcore( 'glGetInfoLogARB', obj, maxLength, length, infoLog );

return
