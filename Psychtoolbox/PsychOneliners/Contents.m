% Psychtoolbox:PsychOneliners.
% 
% help Psychtoolbox % For an overview, triple-click me & hit enter.
% 
%
%    AppleVersion            - Get Apple OS version information.
%    AreStructsEqualOnFields - Are two structures the same on the passed list of fields?
%    Ask                     - Display message, get user's response.
%    AssertMex               - Detect missing mex file and exit with error.
%    AssertOpenGL            - Require that Psychtoolbox be based on OpenGL.
%    AssertGLSL              - Require the OpenGL shading language is supported.
%    AssertOSX               - Require that Psychtoolbox be based on OS X.
%    BlackIndex.m            - Returns number that will produce the color black.
%    CatStr                  - Concatenate array or cell array of strings.
%    DescribeComputer        - Print a description of the environment.
%    EnforcePos              - Truncate negative values of a vector to 0.
%    FunctionFolder          - Get full path to folder containing passed function.
%    GetMyCaller             - Returns the name of the calling function.
%    GetString               - Get a string typed at the keyboard.
%    GetSubversionPath       - Return path required to invoke snv.
%    GrayIndex               - Any graylevel from black (0) to white (1).
%    GroupStructArrayByFields - An aid to sorting data kept in structure arrays.
%    hexstr                  - Hex string of lowest 32 bits of any number.
%    ImageToVec              - Convert a grayscale image to vector format.
%    IsLinux                 - Shorthand for testing whether running under Linux.
%    IsOctave                - Shortand for testing whether running under Octave.
%    IsOS9                   - Shorthand for streq(computer,'MAC2').
%    IsOSX                   - Shorthand for streq(computer,'MAC').
%    IsWin                   - Shorthand for streq(computer,'PCWIN').
%    LoadIdentityClut        - Loads the identity CLUT on a specified monitor.
%    log10nw                 - Compute log base 10 without annoying warnings.
%    MacModelName            - Mac model name, e.g. 'PowerBook G4 15"'
%    Magnify2DMatrix         - Expand the size of a two-dimensional matrix via entry replication.
%    MakeBeep                - Compute a beep of specified frequency and duration, for Snd.
%    MakeCosImage            - Make a cosinusoidal image.
%    MakeSincImage           - Make a sinc image.
%    MakeSineImage           - Make a sinusoidal image.
%    MapIndexColorThroughClut - Convert an index color image and clut to a true color image.
%    NameBytes               - Nicely format memory size for human readers.
%    NameFrequency           - Nicely format clock rate for human readers.
%    OSName                  - Convential English-language name of your operating system.
%    PackColorImage          - Pack three color planes into one m by n by three matrix.
%    PsychtoolboxRoot        - Robust way to get path to Psychtoolbox folder, even if renamed.
%    RemoveSVNPaths          - Removes .svn folders from the path.
%    sca                     - Shorthand for Screen('CloseAll').  Using this is a good way to make your code obscure.
%    ScreenDacBits           - What is precision of the graphics boardDACs. Currently returns 8 always.
%    Stopwatch               - Time intervals.
%    streq                   - strcmp.
%    TextBounds              - Draw string, return enclosing rect.
%    TextCenteredBounds      - Draw string, centered, return enclosing rect.
%    UnpackColorImage        - Extract three color planes from an m by n by 3 color image.
%    VecToImage              - Convert a grayscale image from vector to image format.
%    WhiteIndex              - Returns number that will produce the color white.
%    WrapString              - Word wrap (break into lines).
%
% The following is a list of old one-liners that might some day be updated
% from PTB-2, but haven't been yet.
%
%   BlankingInterruptRate - Used by PsychBasic FrameRate.
%   ClutDefault           - Returns standard clut for screen at any pixelSize.
%   CmdWinToUpperLeft     - Bring Command window forward, saving Screen window.
%   DescribeScreen        - Print a description of the screen's video driver.
%   DescribeScreenPrefs   - Print more about the screen's video driver.
%   GammaIdentity         - Returns an identity gamma table appropriate to the screen's dacSize.
%   GetEchoNumber         - Get a number typed on-screen.
%   GetEchoString         - Get a string typed on-screen.
%   GetNumber             - Get a number typed at the keyboard.
%   IsDownArrow           - Is char the down arrow?
%   IsLeftArrow           - Is char the left arrow?
%   IsRightArrow          - Is char the right arrow?
%   IsUpArrow             - Is char the up arrow?
%   IsInOrder             - Are the two strings in alphabetical order?
%   IsPopCharProInstalled - Is the Control Panel "PopChar Pro" installed?
%   MaxPriorityGetSecs    - Figure out the maximum priority compatible with GetSecs. Slow.
%   NearestResolution     - Returns the screen resolution most similar to your wish.
%   ScreenClutSize        - How many entries in the graphic card Color Lookup Table?
%   ScreenUsesHighGammaBits - Does this card use the high 10 bits of the gamma values?
%   SCREENWinToFront      - Bring Screen window back in front of Command window.
%   SetResolution         - Sets the screen resolution: width, height, hz, and pixelSize.
%   ShowTiff              - Show a TIFF file, calibrated.




