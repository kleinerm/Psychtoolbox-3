% Psychtoolbox:PsychOneliners.
% 
% help Psychtoolbox % For an overview, triple-click me & hit enter.
% 
%
%    AppleVersion            - Get Apple OS version information.
%    AreStructsEqualOnFields - Are two structures the same on the passed list of fields?
%    Ask                     - Display message, get user's response.
%    AssertMex               * Detect missing mex file and exit with error.
%    AssertOpenGL            * Require that Psychtoolbox be based on OpenGL.
%    AssertOSX               * Require that Psychtoolbox be based on OS X.
%    BlackIndex.m            - Returns number that will produce the color black.
%    CatStr                  - Concatenate array or cell array of strings.
%    DescribeComputer        - Print a description of the environment.
%    GetMyCaller             * Returns the name of the calling function.
%    GetString               - Get a string typed at the keyboard.
%    GrayIndex               - Any graylevel from black (0) to white (1).
%    hexstr                  - Hex string of lowest 32 bits of any number.
%    IsOS9                   - Shorthand for streq(computer,'MAC2').
%    IsOSX                   - Shorthand for streq(computer,'MAC').
%    IsWin                   - Shorthand for streq(computer,'PCWIN').
%    MacModelName            * Mac model name, e.g. 'PowerBook G4 15"'
%    MakeBeep                - Compute a beep of specified frequency and duration, for Snd.
%    NameBytes               * Nicely format memory size for human readers.
%    NameFrequency           * Nicely format clock rate for human readers.
%    OSName                  * Convential English-language name of your operating system.
%    PsychtoolboxRoot        - Robust way to get path to Psychtoolbox folder, even if renamed.
%    Stopwatch               - Time intervals.
%    streq                   - strcmp.
%    TextBounds              - Draw string, return enclosing rect.
%    TextCenteredBounds      - Draw string, centered, return enclosing rect.
%    WhiteIndex              - Returns number that will produce the color white.
%    WrapString              - Word wrap (break into lines).


%from OS 9 

%   AppleVersion          - Get Apple OS version information.
%   AreStructsEqualOnFields - Are two structures the same on the passed list of fields?
%   Ask                   - Display message, get user's response.
%   BlackIndex            - Returns number that will produce the color black.
%   BlankingInterruptRate - Used by PsychBasic FrameRate.
%   CatStr                - Concatenate array or cell array of strings.
%   ClutDefault           - Returns standard clut for screen at any pixelSize.
%   CmdWinToUpperLeft     - Bring Command window forward, saving Screen window.
%   DescribeComputer      - Print a description of the environment.
%   DescribeScreen        - Print a description of the screen's video driver.
%   DescribeScreenPrefs   - Print more about the screen's video driver.
%   EnforcePos            - Truncate negative values of a vector to 0.
%   GammaIdentity         - Returns an identity gamma table appropriate to the screen's dacSize.
%   GetEchoNumber         - Get a number typed on-screen.
%   GetEchoString         - Get a string typed on-screen.
%   GetNumber             - Get a number typed at the keyboard.
%   GetString             - Get a string typed at the keyboard.
%   GrayIndex             - Any graylevel from black (0) to white (1).
%   GroupStructArrayByFields - An aid to sorting data kept in structure arrays.
%   ImageToVec            - Convert a grayscale image to vector format.
%   IsDownArrow           - Is char the down arrow?
%   IsLeftArrow           - Is char the left arrow?
%   IsRightArrow          - Is char the right arrow?
%   IsUpArrow             - Is char the up arrow?
%   IsInOrder             - Are the two strings in alphabetical order?
%   IsOS9                 - Shorthand for streq(computer,'MAC2').
%   IsOSX                 - Shorthand for streq(computer,'MAC').
%   IsWin                 - Shorthand for streq(computer,'PCWIN').
%   IsPopCharProInstalled - Is the Control Panel "PopChar Pro" installed?
%   log10nw               - Compute log base 10 without annoying warnings.
%   MakeBeep              - Compute a beep of specified frequency and duration, for Snd. 
%   MakeCosImage          - Make a cosinusoidal image.
%   MakeSincImage         - Make a sinc image.
%   MakeSineImage         - Make a sinusoidal image.
%	MapIndexColorThroughClut - Convert an index color image and clut to a true color image.
%   MaxPriorityGetSecs    - Figure out the maximum priority compatible with GetSecs. Slow.
%   NearestResolution     - Returns the screen resolution most similar to your wish.
%   PackColorImage        - Pack three color planes into one m by n by three matrix.
%   PsychtoolboxRoot      - Robust way to get path to Psychtoolbox folder, even if renamed.
%   ScreenClutSize        - How many entries in the graphic card Color Lookup Table?
%   ScreenDacBits         - What is the precision of the graphics board DACs (8 or 10 bits)?
%   ScreenUsesHighGammaBits - Does this card use the high 10 bits of the gamma values?
%   SCREENWinToFront      - Bring Screen window back in front of Command window.
%   SetResolution         - Sets the screen resolution: width, height, hz, and pixelSize.
%   ShowTiff              - Show a TIFF file, calibrated.
%   streq                 - strcmp.
%   TextBounds            - Draw string, return enclosing rect.
%   TextCenteredBounds    - Draw string, centered, return enclosing rect.
%   UnpackColorImage      - Extract three color planes from an m by n by 3 color image.
%   VecToImage            - Convert a grayscale image from vector to image format.
%   WhiteIndex            - Returns number that will produce the color white.
%   WrapString            - Word wrap (break into lines).
