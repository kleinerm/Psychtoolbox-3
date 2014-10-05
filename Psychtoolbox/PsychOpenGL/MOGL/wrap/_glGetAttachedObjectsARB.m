function [ count, obj ] = glGetAttachedObjectsARB( containerObj, maxCount )

% glGetAttachedObjectsARB  Interface to OpenGL function glGetAttachedObjectsARB
%
% usage:  [ count, obj ] = glGetAttachedObjectsARB( containerObj, maxCount )
%
% C function:  void glGetAttachedObjectsARB(GLhandleARB containerObj, GLsizei maxCount, GLsizei* count, GLhandleARB* obj)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

count = int32(0);
obj = double(0);

moglcore( 'glGetAttachedObjectsARB', containerObj, maxCount, count, obj );

return
