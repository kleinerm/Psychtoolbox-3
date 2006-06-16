/*
 * GetCharJava.java is a 1.4 example that requires
 * no other files.
 *
 * Allen.Ingling@nyu.edu
 *
 * Derived from Sun's Java demo KeyEventDemo.java available here:
 * http://java.sun.com/docs/books/tutorial/uiswing/events/example-1dot4/KeyEventDemo.java
 */
 
 
 /* HISTORY
 
	2/2006		awi		Wrote it.
	3/28/2006	awi		Added circular buffer and buffer overflow detection. 
	
 
 /*
	Done:
	1.  *COMPLETE* Add a method wich detects focus loss so that we can restore focus within the GetChar.m wrapper when it is lost.
	2.  *COMPLETE* the ring buffer implementation; Modify .m wrappers to handle -1 overflow flag returned by numChars, peekChar and getChar methods.
	7.  *COMPLETE* Change the window style to get rid of the close button because that kills MATLAB instantly without prompting for file save. 
	8.  *COMPLETE* Try to set Alpha to make the window invisible.  Or to that some other way. 
	9.  *COMPLETE* Remove the window's contents, it does not need to display that.
	13. *UNNECESSARY* Try changing the frame thickness of the window.  This might reduce its size, seems larger than it needs to be to hold the text box.   
	3.  *COMPLETE* Test that it works during WaitSecs.  Add that test to GetCharTest.
	4.  *UNNECESSARY* Complete the final section of Getchar Test which tests for interaction with mouse clicks on fsw.


	To Do:
	5. Write EventAvail. Check svn logs to see if its gone missing, or never existed in OS X.    
	6. Test it under priority to see if it disturbs blit timing.
	10. Return a timestamp and reconcile units with GetSecs.  The second returns argument of GetChar in OS 9 looks like this:
	
					ticks: 5760808
					secs: 1.4681e+05
					address: 2
					mouseButton: 0
					alphaLock: 0
					commandKey: 0
					controlKey: 0
					optionKey: 0
					shiftKey: 0
					
		Appropriate Java event methods might be:
		
		GetWhen, isAltDown, isControlDown, isMetaDown, isShiftDown.  See especially getModifiersEx().

	11. Right an installer to set the Java path instead of issuing instructions.
	12. Try recording which window had focus before setting it to the GetChar window, then restore focus after we get the character. A general-purpose cocoa mex file might
		work for this or perhaps Java offers something useful. 
 
 
 */
 
 /*
 
	NOTES
	
	3/9/2006	awi		It looks like we can wire straight into the queue without using a window.  See documentation for KeyEventDispater here:
						http://java.sun.com/j2se/1.5.0/docs/api/java/awt/KeyEventDispatcher.html
						
	3/9/2006	awi		Useful documentation:
	
						JFrame:
						http://java.sun.com/j2se/1.5.0/docs/api/java/awt/Frame.html
						
	3/28/2006	awi		Glass panes look useful:
						http://java.sun.com/j2se/1.5.0/docs/api/javax/swing/RootPaneContainer.html#setGlassPane(java.awt.Component)
						http://java.sun.com/docs/books/tutorial/uiswing/components/rootpane.html						
						
						
						
*/ 

import javax.swing.*;
import java.awt.event.*;
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.GraphicsConfiguration;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;
import java.awt.Rectangle;

public class GetCharJava extends JPanel 
                          implements KeyListener,
                                     ActionListener {
    JTextArea displayArea;
    JTextField typingArea;
    static final String newline = "\n";
	static JFrame frame;
	static char[] characterBuffer;
	static int[] eventModifiersBuffer;
	static long[] eventTimesBuffer;
	int characterBufferTail=0;
	int characterBufferHead=0;
	int characterBufferHeadStepped=0;
	int lastCharModifier=0;
	long lastCharTime=0;
	boolean bufferOverflowFlag=false;	
	char emptyChar='\0';
	int	queueSizeChars=1000;						//make this small to test the ring buffer and overflow warnings


    public GetCharJava() {
        super(new BorderLayout());

//        JButton button = new JButton("Clear");
//        button.addActionListener(this);

//      typingArea = new JTextField(20);
//        typingArea = new JTextField(1);
        typingArea = new JTextField(0);
		Font typingAreaFont= new Font("Times", 0, 1);
		typingArea.setFont(typingAreaFont);

		//typingArea.getFont();
//		typingArea.setSize(1,1);
        typingArea.addKeyListener(this);

        //Uncomment this if you wish to turn off focus
        //traversal.  The focus subsystem consumes
        //focus traversal keys, such as Tab and Shift Tab.
        //If you uncomment the following line of code, this
        //disables focus traversal and the Tab events will
        //become available to the key event listener.
        typingArea.setFocusTraversalKeysEnabled(false);

//        displayArea = new JTextArea();
//        displayArea.setEditable(false);
//        JScrollPane scrollPane = new JScrollPane(displayArea);
//        scrollPane.setPreferredSize(new Dimension(375, 125));

        add(typingArea, BorderLayout.PAGE_START);
//        add(scrollPane, BorderLayout.CENTER);
//        add(button, BorderLayout.PAGE_END);
		
		//setup the ring buffer by allocating memory
		characterBuffer= new char[queueSizeChars];
		eventModifiersBuffer= new int[queueSizeChars]; 
		eventTimesBuffer= new long[queueSizeChars];  
    }

    /** Handle the key typed event from the text field. */
    public void keyTyped(KeyEvent e) {
//        displayInfo(e, "KEY TYPED: ");
		recordKey(e);	
    }

    /** Handle the key pressed event from the text field. */
    public void keyPressed(KeyEvent e) {
//        displayInfo(e, "KEY PRESSED: ");
    }

    /** Handle the key released event from the text field. */
    public void keyReleased(KeyEvent e) {
//        displayInfo(e, "KEY RELEASED: ");
    }

    /** Handle the button click. */
    public void actionPerformed(ActionEvent e) {
        //Clear the text components.
//        displayArea.setText("");
        typingArea.setText("");

        //Return the focus to the typing area.
        typingArea.requestFocusInWindow();
    }
	

	public void windowFocusOn(){
		frame.toFront();
	}
	
	public void typingAreaFocusOn(){
		typingArea.requestFocusInWindow();
	}
	
	public void windowFocusOff(){
		frame.toBack();
	}
	

	public String getTitle(){
		return(frame.getTitle());
	}
	
	public void sayHello(){
		System.out.println("Hello World!");
	}
	

    /*
     * We have to jump through some hoops to avoid
     * trying to print non-printing characters 
     * such as Shift.  (Not only do they not print, 
     * but if you put them in a String, the characters
     * afterward won't show up in the text area.)
     */
/*
    protected void displayInfo(KeyEvent e, String s){
        String keyString, modString, tmpString,
               actionString, locationString;

        //You should only rely on the key char if the event
        //is a key typed event.
        int id = e.getID();
        if (id == KeyEvent.KEY_TYPED) {
            char c = e.getKeyChar();
            keyString = "key character = '" + c + "'";
        } else {
            int keyCode = e.getKeyCode();
            keyString = "key code = " + keyCode
                        + " ("
                        + KeyEvent.getKeyText(keyCode)
                        + ")";
        }

        int modifiers = e.getModifiersEx();
        modString = "modifiers = " + modifiers;
        tmpString = KeyEvent.getModifiersExText(modifiers);
        if (tmpString.length() > 0) {
            modString += " (" + tmpString + ")";
        } else {
            modString += " (no modifiers)";
        }

        actionString = "action key? ";
        if (e.isActionKey()) {
            actionString += "YES";
        } else {
            actionString += "NO";
        }

        locationString = "key location: ";
        int location = e.getKeyLocation();
        if (location == KeyEvent.KEY_LOCATION_STANDARD) {
            locationString += "standard";
        } else if (location == KeyEvent.KEY_LOCATION_LEFT) {
            locationString += "left";
        } else if (location == KeyEvent.KEY_LOCATION_RIGHT) {
            locationString += "right";
        } else if (location == KeyEvent.KEY_LOCATION_NUMPAD) {
            locationString += "numpad";
        } else { // (location == KeyEvent.KEY_LOCATION_UNKNOWN)
            locationString += "unknown";
        }

        displayArea.append(s + newline
                           + "    " + keyString + newline
                           + "    " + modString + newline
                           + "    " + actionString + newline
                           + "    " + locationString + newline);
        displayArea.setCaretPosition(displayArea.getDocument().getLength());
    }
*/
	

	//we add new characters to the head and remove them from the tail
	protected void recordKey(KeyEvent e){
        int id = e.getID();
        if (id == KeyEvent.KEY_TYPED) {
			characterBuffer[characterBufferHead]= e.getKeyChar();
			eventModifiersBuffer[characterBufferHead]= e.getModifiersEx();
			eventTimesBuffer[characterBufferHead]=e.getWhen();
			characterBufferHeadStepped= (characterBufferHead+1) % queueSizeChars; 
			if(characterBufferHeadStepped == characterBufferTail)
				bufferOverflowFlag=true;
			else
				characterBufferHead= characterBufferHeadStepped;																		
		}
	}
									 


	public double numChars(){
		if(bufferOverflowFlag)
			return(-1.0);
		else
			return((double)numCharsLocal());
	}
 


	public int numCharsLocal(){
		if(characterBufferTail > characterBufferHead)
			return(queueSizeChars-characterBufferTail);
		else
			return(characterBufferHead-characterBufferTail);
	}



	public double peekChar(){
		if(bufferOverflowFlag)
			return(-1.0);
		else if(numCharsLocal() > 0){
			return((double)characterBuffer[characterBufferTail]);
		}else{
			return((double)emptyChar);		
		}	
    }
	
	
/*
	public char getChar(){
		if(numChars() > 0){
			char tempChar= characterBuffer[characterBufferTail];
			characterBufferTail= characterBufferTail+1;
			return(tempChar);
		}else{
			return(emptyChar);		
		}	
    }
*/
	public double getChar(){
		if(bufferOverflowFlag){
			return(-1.0);
		}else if(numCharsLocal() > 0){
			char tempChar= characterBuffer[characterBufferTail];
			lastCharModifier= eventModifiersBuffer[characterBufferTail];
			lastCharTime= eventTimesBuffer[characterBufferTail];
			characterBufferTail= (characterBufferTail+1) % queueSizeChars;
			return((double)tempChar);
		}else{
			return((double)emptyChar);		
		}	
    }
	
	/* The result of this function is invalid if GetChar did not return a character */
	public boolean getModifierShift(){
		boolean modifierShiftFlag= (InputEvent.SHIFT_DOWN_MASK & lastCharModifier) == InputEvent.SHIFT_DOWN_MASK;
		return(modifierShiftFlag);
	}
	

	/* The result of this function is invalid if GetChar did not return a character */
	public boolean getModifierOptionAlt(){
		boolean modifierOptionAltFlag= (InputEvent.ALT_DOWN_MASK & lastCharModifier) == InputEvent.ALT_DOWN_MASK;
		return(modifierOptionAltFlag);
	}

	/* The result of this function is invalid if GetChar did not return a character */
	public boolean getModifierControl(){
		boolean modifierControlFlag= (InputEvent.CTRL_DOWN_MASK & lastCharModifier) == InputEvent.CTRL_DOWN_MASK;
		return(modifierControlFlag);
	}
	
	
	public boolean getModifierCommand(){
		boolean modifierCommandFlag= (InputEvent.META_DOWN_MASK & lastCharModifier) == InputEvent.META_DOWN_MASK;
		return(modifierCommandFlag);
	}
	
	public double getModifierValue(){
		return(lastCharModifier);
	}
	
	public long getEventTime(){
		return(lastCharTime);
	}
	
	
	public double getSystemTime(){
		return((double)System.currentTimeMillis());
	}
	

/*
	public double getModifierMouseButton(){
		int mouseButtonsSum=0;
		if( (InputEvent.BUTTON1_DOWN_MASK & lastCharModifier) == InputEvent.BUTTON1_DOWN_MASK )
			mouseButtonsSum=mouseButtonsSum+1;
		if( (InputEvent.BUTTON2_DOWN_MASK & lastCharModifier) == InputEvent.BUTTON2_DOWN_MASK )
			mouseButtonsSum=mouseButtonsSum+2;
		if( (InputEvent.BUTTON3_DOWN_MASK & lastCharModifier) == InputEvent.BUTTON3_DOWN_MASK )
			mouseButtonsSum=mouseButtonsSum+4;
		return(mouseButtonsSum);
	}
*/
	public boolean getModifierMouseButton1(){
		boolean modifierShiftFlag= (InputEvent.BUTTON1_DOWN_MASK & lastCharModifier) == InputEvent.BUTTON1_DOWN_MASK;
		return(modifierShiftFlag);
	}
	
	public String getModifierNames(){
		return(InputEvent.getModifiersExText(lastCharModifier));
	}
	

	public void flushChars(){
		bufferOverflowFlag=false;
		characterBufferHead=characterBufferTail;
	}
	
	public boolean isWindowFocused(){
		return(frame.isFocused());
	}
	
	public double getQueueLength(){
		return((double)(queueSizeChars-1));
	}
	
	
	
    /**
     * Create the GUI and show it.  For thread safety,
     * this method should be invoked from the
     * event-dispatching thread.
     */
    //private static void createAndShowGUI() {
	/*
     public static void createAndShowGUI() {
        //Make sure we have nice window decorations.
        JFrame.setDefaultLookAndFeelDecorated(true);

        //Create and set up the window.
        JFrame frame = new JFrame("GetCharJava");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

        //Create and set up the content pane.
        JComponent newContentPane = new GetCharJava();
        newContentPane.setOpaque(true); //content panes must be opaque
        frame.setContentPane(newContentPane);

        //Display the window.
        frame.pack();
        frame.setVisible(true);
    }
	*/
	
	
	public static double getVersion(){
		return(0.1);
	}
	
	 public static JComponent createAndShowGUI() {
	 		
		// Make sure we have nice window decorations.
		// JFrame.setDefaultLookAndFeelDecorated(true);
		JFrame.setDefaultLookAndFeelDecorated(false);

		// Create and set up the window.
		frame = new JFrame("GetCharJava");
		
		// note: it seems necessary to both use "setDefaultLookAndFeelDecorated(false)" and setUndecorated(true) to get a plain window
		// sans border and close buttons. 
		frame.setUndecorated(true);
		
		// hide the frame  beneath the menu bar.    
		GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
		GraphicsDevice[] gs = ge.getScreenDevices();
		GraphicsDevice gd = gs[0];		//graphics device 0 is the primary display.
		GraphicsConfiguration gc = gd.getDefaultConfiguration();
		Rectangle gcRectBounds = gc.getBounds();
//		frame.setBounds(100, 100, 1, 1);
		frame.setBounds((int)gcRectBounds.getX(), (int)gcRectBounds.getY(), 1, 1);     
		
		// frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		// This should not longer be necessary because we did away with the close button, but it does not seem to hurt.  
		frame.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
		
		// Create and set up the content pane.
		JComponent newContentPane = new GetCharJava();
		newContentPane.setOpaque(true); //content panes must be opaque
		frame.setContentPane(newContentPane);

		// Display the window.
		frame.pack();
		frame.setVisible(true);

		
		//return this object
		return(newContentPane);
	}

/*
    public static void main(String[] args) {
        //Schedule a job for the event-dispatching thread:
        //creating and showing this application's GUI.
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                createAndShowGUI();
            }
        });
    }
*/


    public static void main() {
        //Schedule a job for the event-dispatching thread:
        //creating and showing this application's GUI.
		
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                createAndShowGUI();
            }
        });
    }
	


}
