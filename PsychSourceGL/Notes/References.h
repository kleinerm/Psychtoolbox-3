/*
	References.h
	

	PLATFORMS:
	
		Only OS X
		
	AUTHORS:
	
		Allen Ingling		awi		Allen.Ingling@nyu.edu

	HISTORY
		10/28/04	awi		Created.  
	 	
	DESCRIPTION:
	
		This file holds lightly annotated links to documentation useful for writing the Psychtoolbox.
		
		Xcode recognizes URLs and passes them to your web browser; just click!     

*/


/*

References:

The guide it Interprocess Communication:
http://developer.apple.com/documentation/MacOSX/Conceptual/OSX_Technology_Overview/MacOSXTechnology/chapter_4_section_5.html

How to use Cocao from a mex Psychtoolbox:
http://developer.apple.com/documentation/Cocoa/Conceptual/CarbonCocoaDoc/index.html

Here are constants for mouse buttons:
http://developer.apple.com/documentation/Carbon/Conceptual/Carbon_Event_Manager/Tasks/chapter_18_section_10.html
This might tell us the number of buttons the on the mouse so we know how many to return from GetCurrentEventState
http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/qsref_main/data_type_2.html#//apple_ref/c/tdef/CGButtonCount
GetCurrentEventButtonState vs. GetCurrentButtonState from the CarbonEventsCore.h header file:
	  GetCurrentEventButtonState returns the queue-synchronized button
      state. It is generally better to use this API than to use the
      Button function or the GetCurrentButtonState function (which
      return the hardware state).
Relevant to porting OS 9 GetMouse to OS X GetMouse:
	"Important:  The Classic Event Manager includes a number of functions that let you poll the state of the primary mouse button. You should avoid using these functions
	(Button, GetMouse, StillDown, WaitMouseUp) (especially on Mac OS X), as they use excessive processor time and slow down the system. Instead of using StillDown or 
	WaitMouseUp, you should use TrackMouseLocation or TrackMouseRegion, which are discussed in “Tracking Mouse Movements”. On Mac OS X 10.2 and later, if you need the
	current button state, you should use GetCurrentEventButtonState (described in “Queue-Synchronized Events (Mac OS X v.10.2 and Later)”)instead of Button. In most 
	cases you’re less interested in the instantaneous state of the button than in its transitions from up to down or vice versa, so it’s better to keep track of the 
	button state with mouse-down and mouse-up events than to poll it directly. This is especially true in the common situation where you want to track the mouse’s 
	movements and take some repeated action for as long as the button is held down.
http://developer.apple.com/documentation/Carbon/Conceptual/Carbon_Event_Manager/Tasks/chapter_18_section_10.html
So looks like to read the buttons we should use GetCurrentEventButtonState
http://developer.apple.com/documentation/Carbon/Conceptual/Carbon_Event_Manager/Tasks/chapter_18_section_7.html
Discussion in a game developers forum about problems with getting the mouse position in Quartz fullscreen mode:
http://www.idevgames.com/forum/archive/index.php/t-7570.html
If we go with trying to use Quartz, it looks like the relevant functions are CGAssociateMouseAndMouseCursorPosition, 
CGWarpMouseCursorPosition, CGDisplayHideCursor, CGDisplayShowCursor, CGDisplayMoveCursorToPoint and CGGetLastMouseDelta.  
A list of Quartz Services function for mouse
http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/index.html?http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/qsref_main/function_group_13.html#//apple_ref/doc/uid/TP30001070-CH201-F17118
Additional Quartz Services functions for mouse are under "Remote Operation:
http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/index.html?http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/qsref_main/function_group_10.html
Carbon documentation for mouse events is here:
http://developer.apple.com/documentation/Carbon/Reference/Carbon_Event_Manager_Ref/index.html?http://developer.apple.com/documentation/Carbon/Reference/Carbon_Event_Manager_Ref/CarbonEventsRef/enum_group_12.html
If we could create a graphport from the CG context to make quickdraw happy and correct the corrdinate problem we could use the eponymous GetMouse.  Maybe
the graphport is not necessary, try that first.    
http://developer.apple.com/documentation/QuickTime/INMAC/MACWIN/imEventMgrRef.11.htm
http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/index.html?http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/qsref_main/function_group_10.html
http://developer.apple.com/documentation/Carbon/Reference/EventManagerLegacyRef/index.html?http://developer.apple.com/documentation/Carbon/Reference/EventManagerLegacyRef/event_mgr_leg_ref/function_group_1.html


A list of all of the  Core Graphics (Quartz) "Services" functions.  Really useful!
http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/index.html?http://developer.apple.com/documentation/GraphicsImaging/Reference/Quartz_Services_Ref/qsref_main/FunctionGroupIndex.html

Demo showing how to extract information about video hardware:
http://developer.apple.com/samplecode/VideoHardwareInfo/VideoHardwareInfo.html

How to avoid a common pitfall in using IOKit.  Don't mistake the kernel headers for the user headers:
http://developer.apple.com/qa/qa2001/qa1107.html

Documentation on OS X real time priority:
http://developer.apple.com/documentation/Darwin/Conceptual/KernelProgramming/scheduler/chapter_8_section_4.html#//apple_ref/doc/uid/TP30000905-CH211-BABHGEFA

Native Carbon data structurs and the CF Type.  "Property Lists" is the keyword:
http://developer.apple.com/documentation/CoreFoundation/Conceptual/CFPropertyLists/index.html
http://developer.apple.com/documentation/CoreFoundation/Reference/CFTypeRef/index.html#//apple_ref/doc/uid/20001203
CF collections which includes Arrays and Dictionaries
http://developer.apple.com/documentation/CoreFoundation/Conceptual/CFCollections/index.html#//apple_ref/doc/uid/10000124i
Ownership of native data structures and related memory managment issues:
http://developer.apple.com/documentation/CoreFoundation/Conceptual/CFMemoryMgmt/index.html#//apple_ref/doc/uid/10000127i
Property lists and XML
http://developer.apple.com/documentation/CoreFoundation/Conceptual/CFPropertyLists/index.html#//apple_ref/doc/uid/10000130i

A Quartz Primer
http://developer.apple.com/documentation/GraphicsImaging/Conceptual/QuartzPrimer/index.html?http://developer.apple.com/documentation/GraphicsImaging/Conceptual/QuartzPrimer/qprimer_main/chapter_1_section_6.html
How to fix Quartz drawing upside down
http://developer.apple.com/qa/qa2001/qa1009.html

Don't call setuid from a shell script
http://www.softlab.ntua.gr/~taver/security/secur11.html

The OS X Font Manager verious useful factoids
http://developer.apple.com/technotes/tn/tn2024.html
Font Manager reference documentation root
http://developer.apple.com/documentation/Carbon/Reference/Font_Manager/index.html#//apple_ref/doc/uid/TP30000051
CFCharacterSet refererence.  What is this ?  CreateBitmapRepresentation might be useful
http://developer.apple.com/documentation/CoreFoundation/Reference/CFCharacterSetRef/index.html#//apple_ref/doc/uid/20001212
Zen mysteries of unicode revealed
http://developer.apple.com/documentation/Carbon/Conceptual/ProgWithTECM/index.html?http://developer.apple.com/documentation/Carbon/Conceptual/ProgWithTECM/tecmgr_about/chapter_1_section_6.html

Example of how to uncover info about video hardware
http://developer.apple.com/samplecode/Video_Hardware_Info/Video_Hardware_Info.html

Apple's Texture Range example
http://developer.apple.com/samplecode/TextureRange/TextureRange.html

Unix shared memory and memory mapping 
http://nscp.upenn.edu/aix4.3html/aixprggd/genprogc/create_shared_mem_wshmat.htm
More ipc through shared memory
http://info2html.sourceforge.net/cgi-bin/info2html-demo/info2html?(ipc)example


Here are some interfacing gadgets:
http://www.curdes.com/index.html
ActiveWire Inc. which still has no drivers for OS X 
http://www.activewireinc.com/
Code Mercenaries I/O warrior
http://www.codemercs.com/E_index.html
LabJack
http://www.labjack.com/












*/