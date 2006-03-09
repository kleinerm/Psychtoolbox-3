function isDefFlag=IsJavaClassDefined(javaClassName)

% isDefFlag=IsJavaClassDefined(javaClassName)
%
% Accept the name of a Java class and return TRUE if the class name is a
% defined variable in the MATLAB environment. Essentially, this tests if
% the named Java class is on the MATLAB Java path.  
%
% The MATLAB Java path is distinct from MATLAB path searched for .m and mex
% files. To change the MATLAB Java path edit the MATLAB classpath.txt file;
% enter "edit classpath.txt" into the MATLAB command window.
%
% see also: PsychJava

% HISTORY
% 3/2/04    awi     Wrote it.



% Psychtoolbox Java classes have the getVersion method, so we look for the
% designated Java class by trying to invoke its getVerision method.  If it
% succeeds, then the class is there so return TRUE.  If it fails, then
% either we have a Java class without the getVersion method, or else the
% class itself is not available.  We distinguish between those two
% conditions according to the error messsage and set isDefFlag accordingly.

global foobar;

isDefFlag=1;
try
    callStr=[javaClassName '.getVersion();'];
    eval(callStr);
catch
    theError=lasterr;
    foobar=theError;
    errorForMissingMethod= ['The class ' javaClassName ' has no property or method named ''getVersion''.'];
    errorForMissingClass= ['Error using ==> eval' 10 'Undefined variable "' javaClassName '" or class "' javaClassName '.getVersion".'];
    missingMethodFlag= strcmp(theError, errorForMissingMethod);
    missingClassFlag= strcmp(theError, errorForMissingClass);
    if ~(missingMethodFlag | missingClassFlag)
        error(['Error unknown to function "IsJavaClassDefined" when invoking ' javaClassName '.getVersion().']);  
    end
    isDefFlag= missingMethodFlag;
end

