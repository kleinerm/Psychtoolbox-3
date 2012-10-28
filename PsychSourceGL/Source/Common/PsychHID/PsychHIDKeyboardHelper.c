/*
  PsychToolbox3/Source/Common/PsychHID/PsychHIDKeyboardHelper.c		
  
  PROJECTS: PsychHID only.
  
  PLATFORMS:  All
  
  AUTHORS:
 
  mario.kleiner@tuebingen.mpg.de    mk
      
  HISTORY:
  
  TO DO:

*/

#include "PsychHID.h"

// Following code is transplanted from SCREENGetMouseHelper.c. It used
// to live inside Screen() for many happy years, and a copy still exists
// there for backwards compatibility and some legacy support.
//
// But mostly this implementation is now used, esp. by the new
// ListenChar()/FlushEvents()/CharAvail()/GetChar() implementation
// for GNU/Octave and Matlab in matlab -nojvm mode. That implementation
// uses our PsychHID keyboard queues to emulate good ol' GetChar()
// functionality when the Java-Based GetChar implementation can't be
// used. For handling of CTRL+C key presses by users by our kbqueue
// thread, we need easy direct access to ConsoleHelper(). For this reason
// we transplant ConsoleHelper() et al. to PsychHID. All other approaches
// proved to fragile.

// Current ListenChar state:
static int listenchar_enabled = 0;
static int stdinpipe[2] = {-1, -1};
FILE* stdininject = NULL;

#if PSYCH_SYSTEM == PSYCH_LINUX
#include <errno.h>
#endif

#if PSYCH_SYSTEM != PSYCH_WINDOWS

/**
 POSIX implementation of _kbhit() for Linux and OS/X:
 Adapted from example code by Morgan McGuire, morgan@cs.brown.edu
 */

#include <stdio.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>

// This implementation also does at/detaching of the stdin stream
// from the controlling tty, control of character echo'ing, buffering,
// and canonical input processing, depending on the requested state
// transitions between different listenchar states.
//
// These are Unix only features.
int _kbhit(void) {
	struct termios          term;
    int                     bytesWaiting;
    static int              current_mode = 0;
    static struct termios   oldterm;
    static int              fd = -1;
    
	// Change of mode requested?
    if (current_mode != listenchar_enabled) {
        
        // Enable of character suppression requested?
        if (!(current_mode & 2) && (listenchar_enabled & 2)) {
            // Switching from unsuppressed to suppressed.
            
            // Get backup of filedescriptor fd of real stdin:
            fd = dup(fileno(stdin));
            
            // Get current termios state of real stdin:
            tcgetattr(fileno(stdin), &term);
            
            // Back it up:
            oldterm = term;
            
            // Disable echo on real stdin:
            term.c_lflag &= ~ECHO;
            tcsetattr(fileno(stdin), TCSANOW, &term);
            
            #if PSYCH_SYSTEM != PSYCH_OSX
            // Detach stdin from controlling tty, redirect to
            // /dev/zero, so it doesn't get any input from now on,
            // regardless what characters go to the terminal:
            freopen("/dev/zero", "r", stdin);
            
            // We are detached: No characters received from terminal,
            // no characters echo'ed by terminal itself.
            
            #else
            // Create a unidirectional Unix communication pipe and
            // return a read [0] and write [1] fildescriptor pair to both
            // ends:
            pipe(stdinpipe);
            
            // Attach the read descriptor [0] to stdin of the runtime.
            // This way, everything written into stdinpipe[1] will appear
            // as input to stdin -> gets fed into our host application:
            dup2(stdinpipe[0], fileno(stdin));
            
            // Attach write descriptor to standard FILE* stdinject for
            // simple use with fwrite() et al.:
            stdininject = fdopen(stdinpipe[1], "a");
            if (stdininject) {
                //setvbuf(stdininject, NULL, _IONBF, 0);
                fprintf(stdininject, "%% Hello world!\n"); fflush(stdininject);
            }
            else printf("STDInject failed! [%s]\n", strerror(errno));
            #endif
        }
        
        // Disable of character suppression requested?
        if ((current_mode & 2) && !(listenchar_enabled & 2)) {
            // Switching from suppressed to unsuppressed:
            
            // Reassign filedescriptor fd of real stdin to stdin from
            // our previous backup, thereby reattaching stdin to the
            // controlling tty:
            dup2(fd, fileno(stdin));
            
            // Close and invalidate our backup fildescriptor:
            close(fd);
            fd = -1;
            
            // Clear potential error conditions:
            clearerr(stdin);
            
            // Restore termios settings from backup as well. This
            // reenables auto-echo'ing of tty if it was enabled
            // beforehand (different between Octave and matlab -nojvm),
            // and flushes all buffers, so we don't get spillover that
            // was cached in some low-level kernel line-discipline buffer:
            tcsetattr(fileno(stdin), TCSAFLUSH, &oldterm);
            
            // We are reattached.
            
            #if PSYCH_SYSTEM == PSYCH_OSX
            // Close our, now unused stdinpipe by closeing both ends:
            close(stdinpipe[1]);
            close(stdinpipe[0]);
            stdinpipe[1] = -1;
            stdinpipe[0] = -1;
            fclose(stdininject);
            stdininject = NULL;
            #endif
        }
        
        // Transition to active character listening?
		if ((current_mode == 0) && (listenchar_enabled > 0) && (fd == -1)) {
            // Yes, and stdin attached to real controlling tty.
            
            // Get current settings of stream:
            tcgetattr(fileno(stdin), &term);
            
			// Disable canonic input processing so we don't need to wait
			// for newline before we get input:
			term.c_lflag &= ~ICANON;
            
            // Apply:
            tcsetattr(fileno(stdin), TCSANOW, &term);
            
            // Disable buffering of characters:
            setbuf(stdin, NULL);            
        }
        
		// New opmode established:
        current_mode = listenchar_enabled;
    }
    
	// Query number of pending characters in stdin stream:
    ioctl(fileno(stdin), FIONREAD, &bytesWaiting);
    return(bytesWaiting);
}

#else
// _kbhit() is part of MS-Windows CRT standard runtime library. We just
// need to include the conio header file. Character suppression does not
// work with it though:
#include <conio.h>
#endif

// Special console handler: Performs functions of ListenChar, FlushEvents,
// CharAvail and GetChar when run in terminal mode without GUI and JavaVM:
void ConsoleInputHelper(int ccode)
{
	int ret;
	
	switch(ccode) {
		case -10:	// ListenChar(0);
			// Disable char listening:
			listenchar_enabled = 0;
			_kbhit();
            break;
            
		case -11:	// ListenChar(1);
			// Enable char listening:
			listenchar_enabled = 1;
			_kbhit();
            break;
            
		case -12:	// ListenChar(2);
			// Enable char listening and suppress output to console:
			listenchar_enabled = 1 + 2;
			_kbhit();
            break;
            
		case -13:	// FlushEvents:
			// Enable char listening:
			listenchar_enabled |= 1;
            
			// Drain queue from all pending characters:
			while (_kbhit()) getchar();
            break;
            
		case -14:	// CharAvail():
			// Enable char listening:
			listenchar_enabled |= 1;
            
			// Return number of pending chars in queue:
			PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) _kbhit());
            break;
            
		case -15:	// GetChar():
			// Enable char listening:
			listenchar_enabled |= 1;
			
			// Retrieve one character: Return if none available.
			// We call _kbhit() once to turn the terminal into non-blocking mode,
			// so it doesn't wait for newlines:
			if (0 == _kbhit()) {
				// Nothing available: Return zero result code:
				ret = 0;
			}
			else {
				// At least one available: Fetch one...
				ret = getchar();
				// Valid?
				if (ret == EOF) {
					// Failed - End of stream or error! Clear error condition:
					clearerr(stdin);
					errno = 0;
					// Return error code -1:
					ret = -1;
				}
			}
			
			// Return whatever we've got:
			PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) ret);
            break;
            
		default:
			PsychErrorExitMsg(PsychError_internal, "Invalid command code encountered in ConsoleInputHelper()! This is an implementation BUG!");
	}
    
	return;
}


static char useString[] = "rc = PsychHID('KeyboardHelper', commandCode)";
static char synopsisString[] =  "Low-Level helper function for controlling keyboard input.\n"
                                "This function accepts a couple esoteric commandCode's to control "
                                "low-level aspects of how keyboards are handled. The commands are "
                                "intentionally undocumented, because only PTB developers should "
                                "deal with this interface.\n";
static char seeAlsoString[] = "";

PsychError PSYCHHIDKeyboardHelper(void)
{   
    int commandCode;
    
    PsychPushHelp(useString, synopsisString, seeAlsoString);
    if (PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none);};

    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));

    // Get and validate command code:
    PsychCopyInIntegerArg(1, kPsychArgRequired, &commandCode);
    
    // Validate against our current weird range, inherited from Screen('GetMouseHelper'):
    if ((commandCode < -15) || (commandCode > -10)) PsychErrorExitMsg(PsychError_user, "Invalid commandCode provided! Outside range [-15 ; -10].");
    
    // Ok. Call the ConsoleInputHelper for that range:
    if ((commandCode >= -15) && (commandCode <= -10)) ConsoleInputHelper(commandCode);
    
    // We're done:
    return(PsychError_none);	
}
