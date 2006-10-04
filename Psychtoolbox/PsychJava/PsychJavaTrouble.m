function PsychJavaTrouble
% PsychJavaTrouble -- What to do if Java-based Psychtoolbox functions fail?
%
% You probably arrived at this help text because of an error in execution
% of one of Psychtoolboxs Java based functions, e.g., ListenChar, CharAvail,
% GetChar or FlushEvents.
%
% There are four common reasons for failure of Java based Psychtoolbox
% functions:
%
% 1. You run Matlab in nojvm mode, i.e, you started Matlab via
% 'matlab -nojvm'. In that case, Matlabs Java virtual machine is disabled
% and so are all Java based Psychtoolbox functions. In that case you need
% to restart Matlab with its Java VM enabled.
%
% 2. You run a very old Matlab version which doesn't have Java support
% (Matlab 5) or whose Java implementation is too old and incompatible with
% Psychtoolbox. In that case you need to upgrade to a more recent Matlab
% version. If you run GNU/Octave you are also out of luck, due to Octaves
% lack of Java support.
% Your Matlab needs to support at least java version 1.4. This means that:
%
% On M$-Windows and GNU/Linux, Matlab version 7.0.0 and later should work,
% because V7 is bundled with java 1.4.2 or 1.5. Matlab 5.x or 6.x won't work,
% unless you manually upgrade Matlab with a new version of Java. See Mathworks
% support knowledgebase for instructions on how to do that.
%
% On MacOS-X 10.3.9 or later, all Matlab versions should work, as Matlab uses
% the java which is installed as part of the operating system. OS-X 10.3.9 provides
% Java 1.4.2, OS-X 10.4.x provides Java 1.5.
%
% 3. The Psychtoolbox/PsychJava/ subfolder of your working copy of
% Psychtoolbox isn't included in Matlabs static Java classpath. The
% Psychtoolbox installer/upgrader (DownloadPsychtoolbox or
% UpdatePsychtoolbox) usually tries to edit the 'classpath.txt' file of
% your Matlab installation in order to add the Psychtoolbox/PsychJava
% subfolder to Matlabs classpath. This procedure may  fail due to
% insufficient access permissions on your system. You can verify this by
% entering 'type classpath.txt' at the Matlab prompt. The printed file
% should contain the path to the PsychJava folder. If it doesn't, you may
% want to edit the file yourself ('which classpath.txt' tells you the
% location of the file) or ask a system administrator to do it for you.
% After editing the file you need to restart Matlab.
%
% If you need a quick temporary fix for the problem, other than editing
% classpath.txt and restarting Matlab, then type 'PsychJavaTrouble' at the
% Matlab command prompt. The function will add the PsychJava folder to the
% dynamic classpath to immediately enable Java based Psychtoolbox
% functions. This fix is temporary however, it needs to be repeated after
% each restart of Matlab. Executing the command will also clear all
% variables and functions from Matlabs workspace (like 'clear all'), so
% adding it to experiment scripts may impair proper working of that
% scripts.
%
% 4. You didn't restart Matlab after the Psychtoolbox installer asked you
% to do so. -> Restart Matlab and retry.
%
% 5. The versions of GetCharJava bundled with Psychtoolbox are incompatible
% with the version of Java installed on your machine or bundled with your
% version of Matlab. If you have a Java SDK installed on your machine,
% Psychtoolbox will try to compile a matching version of GetCharJava.
% This should succeed on OS-X but is unlikely to work on Windows, because
% that system does not have a javac compiler installed by default.
%
% 6. Other reasons: Post to the Psychtoolbox forum and ask for help.
%
% Good luck!

% History:
% 20.09.2006 Written (MK)

if IsOctave
    error('Running on GNU/Octave: Java is not supported on that system. Sorry!');
end

if ~exist('javaaddpath')
    error('Your version of Matlab does not support dynamic Java class path. Sorry!');
end

try
    fprintf('PsychJavaTrouble: Will now try to add the PsychJava folder to Matlabs dynamic classpath...\n');
    javapath = [PsychtoolboxRoot 'PsychJava'];
    javaaddpath(javapath);
    fprintf('PsychJavaTrouble: Added PsychJava folder to dynamic class path. Psychtoolbox Java commands should work now!\n');
    return;
catch
    error('Failed to add PsychJava to classpath for unknown reason!');
    psychrethrow(psychlasterror);
end
