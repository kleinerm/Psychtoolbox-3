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


% Is this Matlab? Is the JVM running?
if psychusejava('desktop')
    % Java enabled on Matlab and not Windows Vista or later. There's work to do.

    % Make sure that the GetCharJava class is loaded.
    if isempty(OSX_JAVA_GETCHAR)
        try
            OSX_JAVA_GETCHAR = AssignGetCharJava;
        catch %#ok<*CTCH>
            error('Could not load Java class GetCharJava! Read ''help PsychJavaTrouble'' for help.');
        end
    end

    if listenFlag
        % Start listening for characters.
        OSX_JAVA_GETCHAR.register;

        % Make sure the Matlab window has keyboard focus:
        if ~IsWinVista && exist('commandwindow') %#ok<EXIST>
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

    % On non-Vista we're done. On Vista and later, we fall-through to the
    % fallback path below, as Java based GetChar() is only useful to
    % suppress character output to the Matlab command window, aka clutter
    % prevention, not for actually recording key strokes.
    if ~IsWinVista
        return;
    end
end

% Running either on Octave, or on Matlab in No JVM mode, or on a MS-Vista
% system or later.

% On all systems but Linux, we need to (ab)use keyboard queues to get anywhere:
if ~IsLinux
    % LoadPsychHID is needed on MS-Windows. It no-ops if called redundantly:
    LoadPsychHID;
    
    if listenFlag > 0
        % Only need to reserve/create/start queue if we don't have it
        % already:
        if ~KbQueueReserve(3, 1, [])
            % Try to reserve default keyboard queue for our exclusive use:
            if ~KbQueueReserve(1, 1, [])
                error('Keyboard queue for default keyboard device already in use by KbQueue/KbEvent functions et al. Use of ListenChar/GetChar etc. and keyboard queues is mutually exclusive!');
            end
            
            % Got it. Allocate and start it:
            PsychHID('KbQueueCreate');
            if IsOSX && IsOctave
                % Need weird quirk for this combo:
                PsychHID('KbQueueRelease');
                PsychHID('KbQueueCreate');                
            end
            PsychHID('KbQueueStart');
        end
    else
        % Does default keyboard queue belong to us?
        if KbQueueReserve(3, 1, [])
            % Yes. Stop and release it:
            PsychHID('KbQueueStop');
            PsychHID('KbQueueRelease');
            KbQueueReserve(2, 1, []);            
        end
    end
end

% This code is only effective on Linux, but doesn't hurt on the other
% systems, so do it:
switch listenFlag
    case 0,
        Screen('GetMouseHelper', -10);
    case 1,
        Screen('GetMouseHelper', -11);
    case 2,
        Screen('GetMouseHelper', -12);
end

return;
