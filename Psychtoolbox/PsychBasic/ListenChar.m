function ListenChar(listenFlag)
% function ListenChar([listenFlag])
%
% Tell the Psychtoolbox function "GetChar" to start or stop listening for
% keyboard input.  By default ListenChar listens when no argument is
% supplied.  Passing 0 will turn off character listening and reset the
% buffer which holds the captured characters. Passing a value of 1 or not
% passing any value will enable listening. Passing a value of 2 will enable
% listening, additionally any output of keypresses to Matlab windows is
% suppressed. Use this with care, if your script aborts with an error,
% Matlab may be left with a dead keyboard until the user presses CTRL+C to
% reenable keyboard input.
%
% This function isn't entirely necessary to turn on listening as calling
% GetChar, CharAvail, or FlushEvents will trigger listening on.  However,
% it is the only method by which to disable listening.
%
% The function does nothing when executed under matlab in -nojvm mode or on
% other runtime environments, e.g., GNU/Octave.
%
% For further explanation see help for "GetChar".  
%
% _________________________________________________________________________
%
% See also: GetChar

% HISTORY
%
% 7/19/05  awi   Wrote it.
% 6/20/06  awi   Use AddPsychJavaPath instead of AssertGetCharJava.
% 8/31/06  cgb   Works with the new character listening system.
% 9/19/06  mk    Modified to work on all Java enabled Matlabs and be a no-op
%                in all other configurations.
% 10/13/06 mk    Support for setting the redispatch-mode of GetChar and
%                friends.
% 05/31/09 mk    Add support for Octave and Matlab in noJVM mode.

global OSX_JAVA_GETCHAR;
 
if nargin == 0
    listenFlag = 1;
elseif nargin > 1
    error('Too many arguments to ListenChar!  See "help ListenChar" for more information');
end

if ~ismember(listenFlag, [0,1,2])
    error('Invalid listenFlag provided!  See "help ListenChar" for more information');
end

% This is Matlab. Is the JVM running?
if psychusejava('desktop')
    % Java enabled. There's work to do.

    % Make sure that the GetCharJava class is loaded.
    if isempty(OSX_JAVA_GETCHAR)
        try
            OSX_JAVA_GETCHAR = AssignGetCharJava;
        catch
            error('Could not load Java class GetCharJava! Read ''help PsychJavaTrouble'' for help.');
        end
    end

    if listenFlag
        % Start listening for characters.
        OSX_JAVA_GETCHAR.register;

        % Make sure the Matlab window has keyboard focus:
        if exist('commandwindow') %#ok<EXIST>
            % Call builtin implementation:
            commandwindow;
            drawnow;
        end

        % Should we block output of characters to Matlab?
        if listenFlag > 1
            % Disable redispatching:
            OSX_JAVA_GETCHAR.setRedispatchFlag(1);
        else
            % Enable redispatching: This is the startup default.
            OSX_JAVA_GETCHAR.setRedispatchFlag(0);
        end
    else
        % Stop listening for characters and clear the buffer.
        OSX_JAVA_GETCHAR.unregister;
        OSX_JAVA_GETCHAR.clear;
        % Enable redispatching:
        OSX_JAVA_GETCHAR.setRedispatchFlag(0);
    end

    return;
end

% Running either on Octave or on OS/X or Linux with Matlab in No JVM mode:
switch listenFlag
    case 0,
        Screen('GetMouseHelper', -10);
    case 1,
        Screen('GetMouseHelper', -11);
    case 2,
        Screen('GetMouseHelper', -12);
end

return;
