/*
 *
 * glm.c -- implementation of glm functions
 *
 * 08-Dec-2005 -- created (RFM)
 * 11-Nov-2006 -- Minimal fixes for IntelMac Matlab beta: Replace mxGetPr by mxGetData where appropriate.
 */

#include "mogltypes.h"

// device information
AGLDevice device;
AGLContext context;
CGDirectDisplayID displayID;
int screenw, screenh, bitsperpixel;
double fps;
GLuint fontList;
int fontID,fontStyle,fontSize;
CFDictionaryRef enterMode;

// general-purpose text buffer
#define TEXT_SIZE 4096
char text[TEXT_SIZE];

void glm_close(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    // delete font
    if( fontList>0 )
        glDeleteLists(fontList,256);

    // tear down context
    aglSetCurrentContext( NULL );
    aglSetDrawable( context, NULL );
    aglDestroyContext( context );

    // restore clut
    CGDisplayRestoreColorSyncSettings();
    
    // release displays
    CGDisplaySwitchToMode(displayID,enterMode);
    CGDisplayRelease(displayID);

	// clear display information
	device=NULL;
    context=NULL;
	displayID=NULL;
	screenw=screenh=0;
	fps=0;

}

void glm_getkeys(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    plhs[0]=mxCreateNumericMatrix(4,1,mxUINT32_CLASS,mxREAL);
    GetKeys((void *)mxGetData(plhs[0]));
    
}

void glm_getmouse(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    // get position
    Point pt;
    GetMouse(&pt);
    plhs[0]=mxCreateDoubleMatrix(1,2,mxREAL);
    double *dmat=mxGetPr(plhs[0]);
    dmat[0]=pt.h;
    dmat[1]=pt.v;
    
    // get button state
    plhs[1]=mxCreateDoubleMatrix(1,1,mxREAL);
    *mxGetPr(plhs[1])=Button();
    
}

void glm_getscreeninfo(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

	// return screen size in pixels
    plhs[0]=mxCreateDoubleMatrix(1,2,mxREAL);
    double *p=mxGetPr(plhs[0]);
    p[0]=(double)screenw;
    p[1]=(double)screenh;
	
	// return frames per secone
    plhs[1]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[1])=(double)fps;
    
	// return bits per pixel
    plhs[2]=mxCreateDoubleMatrix(1,1,mxREAL);
	*mxGetPr(plhs[2])=(double)bitsperpixel;
    
}

void glm_open(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    // get requested device
    int i=1,nRequestedDevice=(int)mxGetScalar(prhs[0]);
    device=GetDeviceList();
    while( i++<nRequestedDevice & device!=NULL )
        device=GetNextDevice(device);
    if( device==NULL )
        mxErrMsgTxt("invalid device number");
    
    // get CGDirectDisplayID for this device
    displayID = QDGetCGDirectDisplayID(device);

    // switch display mode
    enterMode=CGDisplayCurrentMode(displayID);
    if( mxGetScalar(prhs[1])>0 ) {
        boolean_t exact;
        CFDictionaryRef mode=CGDisplayBestModeForParameters(displayID,
            (size_t)mxGetScalar(prhs[3]),(size_t)mxGetScalar(prhs[1]),(size_t)mxGetScalar(prhs[2]),
            &exact);
        if(!exact)
            mxErrMsgTxt("invalid display mode");
        CGDisplayCapture(displayID);
        CGDisplaySwitchToMode(displayID,mode);
    }
    else
        CGDisplayCapture(displayID);

    // find a pixel format
    AGLPixelFormat fmt = aglChoosePixelFormat(&device,1,(const GLint *)mxGetData(prhs[4]));
    if( fmt==NULL )
        mxErrMsgTxt("no matching pixel format");
    
    // create context and enter full-screen mode
    context = aglCreateContext(fmt,NULL);
    aglEnable(context,AGL_FS_CAPTURE_SINGLE);
    aglSetFullScreen(context,0,0,0,0);
    aglSetCurrentContext(context);
    aglDestroyPixelFormat(fmt);

    // record screen information
	CFDictionaryRef dispInfo=CGDisplayCurrentMode(displayID);
	CFNumberRef dispNum;
	// width
	dispNum=CFDictionaryGetValue(dispInfo,kCGDisplayWidth);
	CFNumberGetValue(dispNum,kCFNumberIntType,&screenw);
	// height
	dispNum=CFDictionaryGetValue(dispInfo,kCGDisplayHeight);
	CFNumberGetValue(dispNum,kCFNumberIntType,&screenh);
	// refresh rate
	dispNum=CFDictionaryGetValue(dispInfo,kCGDisplayRefreshRate);
	CFNumberGetValue(dispNum,kCFNumberDoubleType,&fps);
	// bits per pixel
	dispNum=CFDictionaryGetValue(dispInfo,kCGDisplayBitsPerPixel);
	CFNumberGetValue(dispNum,kCFNumberIntType,&bitsperpixel);
    
    int argp=0;
    char *str="cmd";
    glutInit(&argp,&str);
    
}

void glm_packpixels(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    // get image
    double *dmat=mxGetPr(prhs[0]);
    int m=mxGetM(prhs[0]),n=mxGetN(prhs[0])/3;
    int padn=( (4-((3*n)%4)) % 4 );
    
    // create return argument
    int n2=3*n+padn;
    plhs[0]=mxCreateNumericMatrix(n2*m+8,1,mxUINT8_CLASS,mxREAL);
    unsigned char *umat=(unsigned char*)mxGetData(plhs[0]);
    
    // store image format and size
    umat[n2*m  ]=GL_RGB/256;
    umat[n2*m+1]=GL_RGB%256;
    umat[n2*m+2]=GL_UNSIGNED_BYTE/256;
    umat[n2*m+3]=GL_UNSIGNED_BYTE%256;
    umat[n2*m+4]=n/256;
    umat[n2*m+5]=n%256;
    umat[n2*m+6]=m/256;
    umat[n2*m+7]=m%256;
    
    // reformat and reorder (much slower if inner and outer loops exchanged)
    int i,j;
    for(j=0;j<n;j++) {
        for(i=0;i<m;i++) {
            umat[3*j  +n2*((m-1)-i)]=(unsigned char)dmat[i+m*j];
            umat[3*j+1+n2*((m-1)-i)]=(unsigned char)dmat[i+m*(j+n)];
            umat[3*j+2+n2*((m-1)-i)]=(unsigned char)dmat[i+m*(j+2*n)];
        }
    }
    
}

// do this type casting in the m-file wrapper
void glm_setclut(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    int i;
    double *clut=mxGetPr(prhs[0]);
    CGByteValue pnRed[256],pnGreen[256],pnBlue[256];
    for(i=0;i<256;i++) {
        pnRed[i]=  (CGByteValue)clut[i];
        pnGreen[i]=(CGByteValue)clut[i+256];
        pnBlue[i]= (CGByteValue)clut[i+512];
    }
    CGSetDisplayTransferByByteTable(displayID,256,pnRed,pnGreen,pnBlue);
    
}

void glm_setfont(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    // delete previous font
    if( fontList>0 )
        glDeleteLists(fontList,256);
    
    // get font ID, style, size
    fontID=   (int)mxGetScalar(prhs[0]);
    fontStyle=(int)mxGetScalar(prhs[1]);
    fontSize= (int)mxGetScalar(prhs[2]);
    
    // create font
    fontList=glGenLists(256);
    aglUseFont(context,fontID,fontStyle,fontSize,0,256,fontList);

}

void glm_setmouse(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    // set position
    if( mxGetM(prhs[0])>0 ) {
        double *p=mxGetPr(prhs[0]);
        CGPoint pos;
        pos.x=(float)p[0];
        pos.y=(float)p[1];
        CGDisplayMoveCursorToPoint(displayID,pos);
    }
    
    // set visibility
    if( mxGetM(prhs[1])>0 ) {
        if( mxGetScalar(prhs[1])!=0 )
            CGDisplayShowCursor(displayID);
        else
            CGDisplayHideCursor(displayID);
    }

}

void glm_setswapinterval(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    GLint swapinterval=(GLint)mxGetScalar(prhs[0]);
    aglSetInteger( context, AGL_SWAP_INTERVAL, &swapinterval );
    
}

AbsoluteTime entrytime;
unsigned char pausepoint[4];
GLint nRead,nDraw;

void glm_swapbuffers(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
  // create return argument and record entry time
  if( nlhs>0 ) {
    plhs[0]=mxCreateNumericMatrix(1,1,mxUINT64_CLASS,mxREAL);
    if( nlhs>1 ) {
      plhs[1]=mxCreateNumericMatrix(1,1,mxUINT64_CLASS,mxREAL);
      entrytime=UpTime();
    }
  }

  // swap buffers
  aglSwapBuffers(context);

  // do a trivial graphics operation so that execution halts here
  // until buffers are swapped
  if( mxGetScalar(prhs[0])>0 ) {
    glGetIntegerv(GL_READ_BUFFER,&nRead);
    glGetIntegerv(GL_DRAW_BUFFER,&nDraw);
    glReadBuffer(GL_BACK);
    glDrawBuffer(GL_BACK);
    glReadPixels(0,0,1,1,GL_RGBA,GL_UNSIGNED_BYTE,pausepoint);
    glWindowPos2d(0,0);
    glDrawPixels(1,1,GL_RGBA,GL_UNSIGNED_BYTE,pausepoint);
    glReadBuffer(nRead);
    glDrawBuffer(nDraw);
    glFinish();
  }

  // record the time
  if( nlhs>0 ) {
    // record the time
    *(AbsoluteTime *)mxGetData(plhs[0])=UpTime();
    if( nlhs>1 )
      *(long long *)mxGetData(plhs[1])=*(long long *)mxGetData(plhs[0])-*(long long *)&entrytime;
  }
}

void glm_text(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

    // check that a font has been created
    if( fontList==0 )
        mexErrMsgTxt("no font has been chosen");
    
    // get string
    mxGetString(prhs[0],text,TEXT_SIZE);
    
    // draw string
    glListBase(fontList);
    glCallLists(strlen(text),GL_UNSIGNED_BYTE,text);
    
}

void glm_textsize(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    
    // check that a font has been created
    if( fontList==0 )
        mexErrMsgTxt("no font has been chosen");

    // get string
    mxGetString(prhs[0],text,TEXT_SIZE);
    
    // save state and use logic operations to prevent bitmap rendering
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    glEnable(GL_COLOR_LOGIC_OP);
    glLogicOp(GL_NOOP);
    
    // see how much raster position changes when text is drawn
    double rpos[]={ 0, 0 };
    glWindowPos2dv(rpos);
    glListBase(fontList);
    glCallLists(strlen(text),GL_UNSIGNED_BYTE,text);
    glGetDoublev(GL_CURRENT_RASTER_POSITION,rpos);
    
    // restore state
    glPopAttrib();
    
    // return text size in pixels
    plhs[0]=mxCreateDoubleMatrix(1,2,mxREAL);
    double *p=mxGetPr(plhs[0]);
    p[0]=(double)rpos[0];
    p[1]=(double)fontSize;
    
}

// command map:  moglcore string commands and functions that handle them
// *** it's important that this list be kept in alphabetical order, 
//     and that glm_map_count be updated for 
//     each new entry ***
int glm_map_count=13;
cmdhandler glm_map[] = {
{ "glmClose",                       glm_close                           },
{ "glmGetKeys",                     glm_getkeys                         },
{ "glmGetMouse",                    glm_getmouse                        },
{ "glmGetScreenInfo",               glm_getscreeninfo                   },
{ "glmOpen",                        glm_open                            },
{ "glmPackPixels",                  glm_packpixels                      },
{ "glmSetClut",                     glm_setclut                         },
{ "glmSetFont",                     glm_setfont                         },
{ "glmSetMouse",                    glm_setmouse                        },
{ "glmSetSwapInterval",             glm_setswapinterval                 },
{ "glmSwapBuffers",                 glm_swapbuffers                     },
{ "glmText",                        glm_text                            },
{ "glmTextSize",                    glm_textsize                        }};
