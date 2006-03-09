function AssertGetCharJava


% OS X: ___________________________________________________________________
% 
% GetCharJava underlies GetChar and related functions such as CharAvail.
% AssertGetCharJava tests if function GetCharJava is available. If
% GetCharJava is available then return silently.  Otherwise,  give
% instructions for installing GetCharJava on your system and exit with
% error.
%
% All Psychtoolbox functions which require GetCharJava call
% AssertGetCharJava.  Therefore, it is unnecessary to call
% AssertGetCharJava within your own programs.  
%
% Installing GetCharJava requires only adding the PsychJava directory to
% the MATLAB Java class path. See instructions issued by AssertGetCharJava
% for details.
%
% see also: GetChar

% HISTORY
% 3/4/05  awi  Wrote it.

if(IsOSX)
    persistent giveSetPathInstructions;
    if isempty(giveSetPathInstructions)  
        if(~IsJavaClassDefined('GetCharJava'))
            pathToGetCharJava=fileparts(which('GetCharJava.class'));
            fprintf('The Psychtoolbox command "GetChar" requires that the Psychtoolbox\n');
            fprintf('directory "PsychJava" be added to the MATLAB Java path.  The Java\n');
            fprintf('class path is different from the MATLAB path set for .m and mex files\n');
            fprintf('\n');
            fprintf('To set the MATLAB Java class path, open the file "classpath.txt"\n');
            fprintf('by entering the command "edit classpath.txt" in the MATLAB\n');
            fprintf('command window.  Using the MATLAB editor, add the PsychJava\n');
            fprintf('Directory to your path by inserting this line into the classpath\n');
            fprintf('file:\n');
            fprintf('\n');
            fprintf([pathToGetCharJava '\n']);
            fprintf('\n');
            fprintf('Then save the classpath.txt file, exit and restart MATLAB.\n');
            fprintf('\n');
            char=''; time=0;
            error('Directory "PsychJava" not on MATLAB Java classpath.');
        else
            giveSetPathInstructions=0;
        end
    end
end



