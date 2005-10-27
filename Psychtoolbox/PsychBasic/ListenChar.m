function ListenChar(listenFlag)

% function ListenChar([listenFlag])
%
% OS X: ___________________________________________________________________
%
% Tell the Psychtoolbox function "GetChar" to start or stop listening for
% keyboard input.  By default ListenChar listens when no argument is
% supplied.
%
% For further explanation see help for "GetChar".  
%
% _________________________________________________________________________
%
% See also: GetChar

% 7/19/05 awi   Wrote it.  

 

if(IsOSX)
    InitCocoaEventBridge;
    if nargin==1
        if listenFlag
            CocoaEventBridge('ListenChars');
        else
            CocoaEventBridge('StopListenChars');
        end
    else
        CocoaEventBridge('ListenChars');
    end
end


