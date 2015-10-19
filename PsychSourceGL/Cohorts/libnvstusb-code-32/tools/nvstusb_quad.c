/* example program 
 * Copyright (C) 2010 Bjoern Paetzel
 * Copyright (C) 2010 Johann Baudy
 *
 * This program comes with ABSOLUTELY NO WARRANTY.
 * This is free software, and you are welcome to redistribute it
 * under certain conditions. See the file COPYING for details
 * */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <getopt.h>

#include "nvstusb.h"

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glext.h>

#include<X11/Xlib.h>
#include<X11/extensions/xf86vmode.h>

struct nvstusb_context *ctx = 0;

/* USAGE : sudo chrt -r -p 99 nvstusb */
/* Usage */
void usage(void) {
  fprintf(stderr, "nvstusb-quad [firmware]\n");
}
/* Main function */
int main(int argc, char **argv) 
{

  Display *dpy;
  Window win;
  uint i_swap_cnt = 0;
  char const * config_fw = NULL;

  /* Getopt section */
  struct option long_options[] =
  {
    /* These options set a flag. */
    {NULL, 0, 0, 0}
  };

  while (1)
  {
    int c;
    /* getopt_long stores the option index here. */
    int option_index = 0;

    c = getopt_long (argc, argv, "",
        long_options, &option_index);

    /* Detect the end of the options. */
    if (c == -1)
      break;

    switch (c)
    {
    case '?':
    default:
      usage();
      exit(EXIT_FAILURE);
    }
  }

  if (optind < argc)
  {
    while (optind < argc) {
      config_fw = argv[optind++];
    }
  }

  /* Openning X display */
  dpy = XOpenDisplay(0);

  /* Preparing new X window */
  Window s_window;
  static int attributeList[] =
  { GLX_RGBA,
    GLX_DOUBLEBUFFER,
    GLX_RED_SIZE,
    1,
    GLX_GREEN_SIZE,
    1,
    GLX_BLUE_SIZE,
    1,
    None };
  XVisualInfo *vi = glXChooseVisual(dpy, DefaultScreen(dpy), attributeList);
  s_window = RootWindow(dpy, vi->screen);
  XSetWindowAttributes swa;
  swa.colormap = XCreateColormap(dpy, s_window, vi->visual, AllocNone);
  swa.override_redirect = 1;

  /* Create X window 1x1 top left of screen */
  win = XCreateWindow(dpy,
      s_window ,
      0,
      0,
      1,
      1,
      0,
      vi->depth,
      InputOutput,
      vi->visual,
      CWColormap|CWOverrideRedirect,
      &swa);

  XMapWindow(dpy, win);

  /* Create glX context */
  GLXContext glx_ctx = glXCreateContext(dpy, vi, 0, 1);
  glXMakeCurrent(dpy, win, glx_ctx);


  /* Initialize libnvstusb */
  ctx = nvstusb_init(config_fw);
  if (0 == ctx) {
    fprintf(stderr, "could not initialize NVIDIA 3D Stereo Controller, aborting\n");
    exit(EXIT_FAILURE);
  }

  /* Get Vsync rate from X11 */
  XF86VidModeModeLine modeline;
  int pixelclock;
  XF86VidModeGetModeLine( dpy, DefaultScreen(dpy), &pixelclock, &modeline );
  double frameRate=(double) pixelclock*1000/modeline.htotal/modeline.vtotal;
  printf("Vertical Refresh rate:%f Hz\n",frameRate);
  nvstusb_set_rate(ctx, frameRate);

  /* Loop until stop */
  while (1) {

    /* Send swap to usb controler */
    nvstusb_swap(ctx, nvstusb_quad, NULL /*f_swap*/);

    /* Read status from usb controler */
    if(!(i_swap_cnt&0xF)) {
      struct nvstusb_keys k;
      nvstusb_get_keys(ctx, &k);
      if (k.toggled3D) {
        nvstusb_invert_eyes(ctx);
      }
    }

    i_swap_cnt++;
  }
  /* Destroy context */
  glx_ctx = glXGetCurrentContext();
  glXDestroyContext(dpy, glx_ctx);

  /* Denit libnvstusb */
  nvstusb_deinit(ctx);

  return EXIT_SUCCESS;
}
