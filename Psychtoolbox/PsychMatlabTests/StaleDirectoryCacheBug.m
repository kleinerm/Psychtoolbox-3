function StaleDirectoryCacheBug
% 
% Mathworks Case ID:  230202, submitted 5/7/99
% 
% Matlab's directory cache is not updated when you add files to /toolbox/.
% 
% dear mathworks
% 
% Matlab 5.2.1 for Mac. Mac OS 8.5.1. I know you're not developing it now, 
% but it's my impression that you are still tracking bugs, in case you do 
% pick it up again.
% 
% Matlab ought to update or flush its directory cache when it saves a new M 
% function into the Toolbox folder. At present, the user must Quit and 
% restart Matlab in order to get Matlab to notice the new file. (It's my 
% impression that doing a "Set Path" did NOT help, even though I would have 
% expected it to.)
% 
% sincerely yours
% 
% denis pelli
% professor of psychology and neural science at nyu
% premier apple developer
% 
% ...
% 
% There was a long back and forth, as Mathworks initially misunderstood my bug report. 
% This message of mine finally succeeded in clarifying the issue.
% ...
% 
% dear rob
% 
% thanks for responding to my phone query. however, i must begin by noting 
% that there is a procedural problem in how Mathworks handles bug reports. 
% The Mathworks responses usually omit the question. If you had actually 
% seen my question (bug report) you would know that I already understand 
% caching and don't need the (very clear) explanation you provide.
% 
% here is what I wrote to Elana on 5/20/99
% 
% >dear elana
% >
% >thanks for your note. There's a subtle point here; let me try to clarify. 
% >Matlab's behavior at the time of dispatching functions is fast and fine. 
% >It is perfectly true that the only reasonable thing to do at that moment 
% >is look up the function in a cache. The bug is the fact that when Matlab 
% >stores a new file in its Toolbox directory, it KNOWS that this 
% >invalidates the cache and it should, at that time, update the cache. It 
% >is a bug for Matlab to knowingly change the directory and leave the cache 
% >in a wrong state. It would be VERY easy to fix this bug since all that's 
% >required is to call the cache update routine after saving a new file into 
% >the Toolbox directory.
% >
% >could you draw your developers' attention to this? 
% >
% >thanks
% >
% >denis
% 
% Your message below restates the excellent reasons for doing caching. 
% However, like Elana's responses, it fails to address the issue I'm 
% raising. Matlab, when it adds a new function to the cached folder, should 
% update the cache (i.e. call path(path) ). Failing to do this means that 
% Matlab is going to rely on a cache that it knows to be stale, which is 
% plain silly given that it's easy to fix. Could you draw the developer's 
% attention to this?
% 
% thanks for getting back to me so quickly.
% 
% best
% 
% denis
% 
% p.s. I'm not sure how much detail to provide in how to implement the fix. 
% I think it's obvious, but in case it isn't, let me specify. When Matlab 
% saves an M file, it should check to see if it's going to save it in the 
% cached directory. The easiest way to do this would be to look for the 
% (possibly new) function in the cache. If there's a hit then we're fine, 
% go ahead and save it. If there's not a hit and the new function will be 
% stored in that directory then the directory cache should be marked as 
% stale. Either you could add custom code to insert the new file name into 
% the cache, or you could rebuild the entire cache, by calling path(path). 
% Rebuilding the cache would be quite acceptable since this will be 
% triggered only when a new file is added (which is rare), not when an 
% existing file is modified. Thus the needed code is trivial, the 
% performance loss is negligible, and the increase in user-perceived 
% reliability of operation enormously increased. It is hard to describe how 
% incredibly distressing it is to find Matlab saying that a just-saved file 
% does not exist. No amount of documenting of such counter-intuitive 
% behavior will make users "expect" this.
% 
% >This is in response to Case ID:  230202
% >
% >Hello Denis,
% >
% >I am writing in response to the voice mail I left you on May 24, 1999, in
% >which I briefly discussed the reasons you cannot see changes to the toolbox
% >subdirectories.
% >
% >Here is the more detailed information I promised to send you.
% >
% >First, a brief overview:
% >
% >When you enter a function name at the prompt, Matlab checks to see whether
% >the file defining the function has been updated.  This can take a lot of
% >time, and seems unnecessary for functions which change very seldomly, such
% >as those found in the \toolbox subdirectories. Therefore, the status for
% >M-files in these subdirectories are 'cached'.  Note that the files
% >themselves are not compiled and in memory, it's just that Matlab keeps
% >track of what and where these files are.
% >
% >Here's how this works internally:
% >
% >When Matlab compiles a function, it keeps a marker indicating the date when
% >the file last changed.  The next time that function is executed, Matlab
% >needs to decide whether to run the version already compiled and in memory
% >(faster), or to recompile the file (slower). It therefore checks the marker
% >against the date associated with the file.  This file date will have been
% >modified by the operating system if the file was changed.  If they are
% >different, Matlab recompiles the file.  This applies to functions called
% >from within other functions, as well as at the command line.
% >
% >This process applies to all functions.  The difference between user
% >functions and toolbox functions is that Matlab keeps a cache of all of the
% >dates for M-files in the \toolbox directories.  This cache is faster than
% >actually looking at the date on the physical file, and it is only updated
% >when the path is changed.  Thus, even if the \toolbox\function.m file has
% >been changed, the *cache* still indicates that Matlab should use the
% >compiled function in memory.  Hence, confusion arises when you modify these
% >files (which, in general, we recommend that you avoid doing).
% >
% >Now, we realize the for a few people, such as those developing toolboxes,
% >this poses a problem.  However, for the overwhelming majority of our users
% >that frequently run the toolbox functions, and do not save their m-files
% >under the toolbox directory, performance is more important than the
% >accuracy of running the proper file.
% >
% >Suggestions:
% >
% >1) In the rare cases where you modify functions in these directories, or if
% >you think Matlab's path cache has become out of date for any reason, just 
% >type
% >
% >      path(path)
% >
% >   at the Matlab prompt.  This will rebuild the cache.
% >
% >2) If you are going to be modifying files a lot (i.e., are in the process
% >of developing them), avoid putting them in the \toolbox directories.  It's
% >probably not that important to cache their information until they are
% >pretty much stable, at which time you can put them in one of the \toolbox
% >directories permanently.
% >
% >You can create your own subdirectory for this purpose.  If you create a new
% >directory, make sure you add it to the Matlab path.
% >
% >If you have further questions regarding this issue, please be sure to
% >mention the above Case ID Number in your email response to me.
% >
% >Sincerely,
% >
% >Rob Monteiro
% >Technical Support Engineer
% >robertm@mathworks.com
% 
% Hello Denis,
% 
% I am writing in response to your e-mail of May 26, 1999, in which you asked
% about recaching files when saving from a toolbox/ directory.
% 
% Thank you for clarifying your request.  I think I understand what you mean.
%  It seems kind of obvious - if you save a file in the toolbox/ directory
% from the Editor/Debugger, we could simply rebuild the cache at that point.
% 
% This stems back from the days of Matlab 4.2c and earlier, before there was
% an Editor/Debugger.  Since m-files are just text files, they could (and
% still can) be opened and modified in any text editor.  When the file was
% saved, Matlab would have no way of knowing that changes were made to a
% function unless it re-loaded the function.  Because the toolbox/ functions
% were cached (along with the last modified date), the changes would never
% get recognized.
% 
% Now that we have an Editor/Debugger, we know (or can find out) for the most
% part when someone saves a file to a toolbox/ directory.  The only problem
% is if someone chooses not to use the Editor, we will not be able to account
% for this.
% 
% However, I do think this is a good idea, so I have re-opened this issue
% with our development staff to see if they can implement this for a future
% release.
% 
% Thank you bringing this to our attention.
% 
% Sincerely,
% 
% Rob Monteiro
% Technical Support Engineer
% robertm@mathworks.com
% 
% Hi Rob,
% 
% Denis has cc'd me on his exchange with respect to the
% stale directory cache issue.  I just wanted to add
% a few post-scripts.
% 
% a) In version 5.2, typing path(path) is often not
% sufficient to cause Matlab (on the Mac) to recognize
% new files placed in a toolbox folder.  Indeed, the
% only thing that I find works consistently is to 
% delete the Matlab settings file and restart Matlab.
% This may have been fixed in 5.2.1, which brings
% me to my next point.
% 
% b) The updater for 5.2.1 works on some machines
% and not on others.  I am not sure of all the 
% parameters, but the machine where I have never
% gotten it to work has multiple disks.
% 
% c) I agree that having your bug tracking system
% include the original report and to provide the
% entire exchange would be very helpful.  It is
% something I have suggested previously but to
% no avail.
% 
% Best,
% 
% David Brainard
% 
% 
