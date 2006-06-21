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

% HISTORY
%
% 7/19/05 awi   Wrote it.
% 6/20/06 awi   Use AddPsychJavaPath instead of AssertGetCharJava.


% TO DO
%
% 3/5/06 awi  For consistancy between platforms, on Windows and OS 9, implement
%             ListenChar(0) by throwing out characters. For now ListenChar(0)
%             gives errors on those platforms.  Character reading is fully
%             capable on those operating systems without ListenChar(0), but
%             providing makes behavior uniform accross platforms.  

 
% OS 9 and Windows listen without having to place focus on a window, so
% ListenChar is a nullop in those systems.  It is an error on those
% platforms to turn off listening.  It would be better to implement that on
% those platforms than to give and error.  
if IsOSX
    %TO DO implement turning off character listening here.
    AddPsychJavaPath;
    %AssertGetCharJava;
    global PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW
    if isempty(PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW)
        PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW=GetCharJava.createAndShowGUI();
    else
        PSYCHTOOLBOX_OSX_JAVA_GETCHAR_WINDOW.windowFocusOn();
    end
else
    if nargin==1 && ~listenFlag
        error(['Turning off character listening using ListenChar(0) is not yet implemented on ' OSName '\n']);
    end
end



% Below is ListenChar for the Cocoa implementation.  It is
% preserved here in case we need to revert to that. 

% OS 9 and Windows listen without having to place focus on a window, so
% ListenChar is a nullop in those systems.  It is an error on those
% platforms to turn off listening.  It would be better to implement that on
% those platforms than to give an 
% if IsOSX
%     InitCocoaEventBridge;
%     if nargin==1
%         if listenFlag
%             CocoaEventBridge('ListenChars');
%         else
%             CocoaEventBridge('StopListenChars');
%         end
%     else
%         CocoaEventBridge('ListenChars');
%     end
% else
%     if nargin==1 && ~listenFlag
%         error(['Turning off character listening using ListenChar(0) is not yet implemented on ' OSName '\n']);
%     end
% end

