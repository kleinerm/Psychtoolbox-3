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
#include <time.h>
#include <math.h>

#include <getopt.h>

#include "nvstusb.h"

#include <GL/glut.h>

#define ILUT_USE_OPENGL
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include<X11/Xlib.h>
#include<X11/extensions/xf86vmode.h>

ILuint image = 0;
GLuint texture = 0;

struct nvstusb_context *ctx = 0;
float depth = 0.0;
int inverteyes = 1;
void (*pf_draw) (int in_i_eye) = NULL;

/* Config */
int config_verbose = 0;
int config_stereo = 0;
int config_swap = 1;
char * config_file = NULL;


/* Refresh rate calculation */
void print_refresh_rate(void)
{
  static int i_it = 0;
  static uint64_t i_last = 0;
  static uint64_t i_first = 0;
  static uint64_t i_acc_var = 0;
  double f_mean, f_var;

  /* First frame */
  if(i_it == 0) {
    struct timespec s_tmp;
    clock_gettime(CLOCK_REALTIME, &s_tmp);
    i_first = (uint64_t)s_tmp.tv_sec*1000000+(uint64_t)s_tmp.tv_nsec/1000;
    i_last = i_first;
    f_mean = 0;
    f_var = 0;

  } else {
    struct timespec s_tmp;
    clock_gettime(CLOCK_REALTIME, &s_tmp);
    uint64_t i_new = (uint64_t)s_tmp.tv_sec*1000000+(uint64_t)s_tmp.tv_nsec/1000;
    /* Update average */
    f_mean = (double)(i_new-i_first)/(i_it);
    /* Calculate variance */
    i_acc_var += pow((double)(i_new-i_last)-f_mean, 2);
    /* std dev */
    f_var = (double)sqrt(i_acc_var/i_it);
    i_last = i_new;

    /* Display each 512 frame */
    if(i_it % 512 == 0) {
      printf("frame:%d (%0.2f s) mean: %f Hz (%0.2f us) sqrt(var): %0.2f us (%0.1f %%)\n",i_it,f_mean*i_it/1000000.0, 1000000/f_mean, f_mean, f_var, 100.0*f_var/f_mean);
    }
  }
  /* Increment frame counter */
  i_it++;
}

/* Image drawing */
void drawImage(int in_i_eye) {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  float offset;
  if (inverteyes) {
    offset = 0.5*(1-in_i_eye);
  } else {
    offset = 0.5*in_i_eye;
  }

  float f = depth * (in_i_eye*2-1);

  glBegin(GL_QUADS);
  glTexCoord2f(0.0+offset, 0.0);
  glVertex2f(-1.0+f, -1.0);
  glTexCoord2f(0.5+offset, 0.0);
  glVertex2f(1.0+f, -1.0);
  glTexCoord2f(0.5+offset, 1.0);
  glVertex2f(1.0+f, 1.0);
  glTexCoord2f(0.0+offset, 1.0);
  glVertex2f(-1.0+f, 1.0);
  glEnd();
}

/* No Image drawing */
void drawNoImage(int in_i_eye) {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glBegin(GL_QUADS);
  if (in_i_eye - inverteyes) {
    glVertex2f(-1.0, -1.0);
    glVertex2f( 0.0, -1.0);
    glVertex2f( 0.0,  1.0);
    glVertex2f(-1.0,  1.0);
  } else {
    glVertex2f( 0.0, -1.0);
    glVertex2f( 1.0, -1.0);
    glVertex2f( 1.0,  1.0);
    glVertex2f( 0.0,  1.0);
  }
  glEnd();
}

/* GLUT Idle */
void idle() {
  switch(config_stereo) {
  case 1:
    /* Draw Left Buffer */
    glDrawBuffer(GL_BACK_LEFT);
    pf_draw(0);
    /* Draw right buffer */
    glDrawBuffer(GL_BACK_RIGHT);
    pf_draw(1);

    if(config_swap) {
      /* Send double swap (quad buffering) */
      nvstusb_swap(ctx, nvstusb_quad, glutSwapBuffers);
    } else {
    	glutSwapBuffers();
    }
    break;

  case 2:
    /* Draw Left Buffer */
    glDrawBuffer(GL_BACK_LEFT);
    pf_draw(0);
    /* Draw right buffer */
    glDrawBuffer(GL_BACK_RIGHT);
    pf_draw(1);

    /* Stereo thread is running, No need to use nvstusb_swap() */
    glutSwapBuffers();
    break;

  case 0:
    {
      static int i_counter = 0;

      /* Draw either left or right depending on counter */
      glDrawBuffer(GL_BACK);
      pf_draw(i_counter&1);

      /* Send swap command */
      if(config_swap) {
        nvstusb_swap(ctx, i_counter&1, glutSwapBuffers);
      } else {
    	  glutSwapBuffers();
      }

      i_counter++;
    }
    break;
  }

  if(config_swap) {
    /* Gather controler status */
    struct nvstusb_keys k;
    nvstusb_get_keys(ctx, &k);
    if (k.toggled3D && (config_stereo != 2)) {
      inverteyes = !inverteyes;
    }

    if (k.deltaWheel) {
      depth += 0.01 * k.deltaWheel;
    }
  }

  /* Display refresh rate info */
  print_refresh_rate();


}

/* Usage */
void usage(void) {
  fprintf(stderr, "example [options] [stereoscopic image]\n");
  fprintf(stderr, "\t--stereo\t\tEnable Stereo GL\n");
  fprintf(stderr, "\t--stereothread\t\tEnable Stereo GL, swap performed in other thread\n");
  fprintf(stderr, "\t--noswap\t\t Disable USB swap\n");
  fprintf(stderr, "\t--debug \t\tEnable Debug\n");
}


/* Main function */
int main(int argc, char **argv) 
{

  /* Getopt section */
  {
    struct option long_options[] =
    {
      /* These options set a flag. */
      {"verbose",      no_argument,       &config_verbose, 1},
      {"stereo",       no_argument,       &config_stereo,  1},
      {"stereothread", no_argument,       &config_stereo,  2},
      {"noswap",no_argument,       &config_swap,  0},
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
      case 0:
        /* If this option set a flag, do nothing else now. */
        if (long_options[option_index].flag != 0)
          break;
        printf ("option %s", long_options[option_index].name);
        if (optarg)
          printf (" with arg %s", optarg);
        printf ("\n");
        break;

      case '?':
      default:
        usage();
        exit(EXIT_FAILURE);
      }
    }

    if (config_verbose)
      puts ("verbose enabled");

    if (config_stereo)
      puts ("stereo enabled");

    /* Print any remaining command line arguments (not options). */
    if (optind < argc)
    {
      while (optind < argc) {
        config_file = argv[optind++];
      }
    }

  }

  /* GL INIT */
  glutInit(&argc, argv);

  if(config_stereo) {
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_STEREO);
  } else {
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE);
  }

  /* Initialize libnvstusb */
  if(config_swap) {
    ctx = nvstusb_init(NULL);

    if (0 == ctx) {
      fprintf(stderr, "could not initialize NVIDIA 3D Stereo Controller, aborting\n");
      exit(EXIT_FAILURE);
    }

    /* Get Vsync rate from X11 */
    {
      static Display *dpy;
      dpy = XOpenDisplay(0);
      double displayNumber=DefaultScreen(dpy);
      XF86VidModeModeLine modeline;
      int pixelclock;
      XF86VidModeGetModeLine( dpy, displayNumber, &pixelclock, &modeline );
      double frameRate=(double) pixelclock*1000/modeline.htotal/modeline.vtotal;
      printf("Vertical Refresh rate:%f Hz\n",frameRate);
      nvstusb_set_rate(ctx, frameRate);
    }
  }

  /* Case: stereoscopic image */
  if (config_file) {
    /* Initialize tex */
    ilInit();
    ilGenImages(1, &image);
    ilBindImage(image);
    ilLoad(IL_JPG, config_file);
    float w = ilGetInteger(IL_IMAGE_WIDTH)/2;
    float h = ilGetInteger(IL_IMAGE_HEIGHT);
    float aspect = w/h;

    /* Create window */
    glutInitWindowSize(512*aspect, 512);
    glutCreateWindow("3dv");

    /* Init GL with texture */
    ilutRenderer(ILUT_OPENGL);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    ilutGLBindTexImage();
    ilutGLBuildMipmaps();
    glEnable(GL_TEXTURE);

    /* Set draw callback */
    pf_draw = drawImage;

    /* Case: No image */
  } else {
    /* Create window */
    glutInitWindowSize(512, 512);
    glutCreateWindow("3dv");

    /* Set draw callback */
    pf_draw = drawNoImage;
  }

  /* If thread stereo selected, start nvstusb thread */
  if((config_stereo == 2) && config_swap) {
    nvstusb_start_stereo_thread(ctx);
  }

  /* Set idle function */
  glutIdleFunc(idle);

  /* Main Loop */
  glutMainLoop();

  if(config_swap) {
    /* Denit libnvstusb */
    nvstusb_deinit(ctx);
  }

  return EXIT_SUCCESS;
}
