function osNameStr=OSName

% sysNameStr=OSName
%
% Return the convential English-language name for your operating system (OS).
% OSName is useful in constructing error message strings which refer to
% particular operating systems. System name strings as returned by the MATLAB
% command "computer" are unsuitable for this purpose because they are
% abbreviations, not names.  
% 
% see also: computer, IsOS9, IsOSX, IsWin, MacModelName, DescribeComputer

% HISTORY
%
% 3/5/06  awi  Wrote it.  For use in ListenChar.


cNameStr=computer;
if streq(cNameStr, 'PCWIN')
    osNameStr='Windows';
elseif streq(cNameStr, 'SOL2')
    osNameStr='Solaris';
elseif streq(cNameStr, 'HPUX')
    osNameStr='HPUX';
elseif streq(cNameStr, 'GLNX86')
    osNameStr='Linux';
elseif streq(cNameStr, 'GLNXA64')
    osNameStr='Linux';
elseif streq(cNameStr, 'MAC')
    osNameStr='OS X';
elseif streq(cNameStr, 'MAC2')
    osNameStr='OS 9';
else
    osNameStr='Unknown OS';
end



