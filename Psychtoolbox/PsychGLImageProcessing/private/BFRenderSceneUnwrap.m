%Render the Scene
% function [] = BFRenderScene(whichEye, theta, rotatev, windowPtr, depthtex_id, depthtex_handle, depthplane)
% 
%     global GL;
%     global IPD;
    %%Render an eye
        % Light properties


    % Material properties
    materialAmb     = [1 1 1 1];
    materialDiff    = [1 1 1 1];
    materialSpec    = [1.0 0.2 0.0 1];
    materialShin    = 50.0;

    % Sphere colors
    matSphereAmb    = builtin('single',materialAmb);
    matSphereDiff   = builtin('single',materialDiff);

    % Cube colors
    matCubeAmb      = builtin('single',[1 1 1 1]);
    matCubeDiff     = builtin('single',[1 1 1.0 1]);
    
    

    Screen('SelectStereoDrawBuffer', windowPtr, whichEye);
    Screen('BeginOpenGL', windowPtr);

    

    

    
    
    
    
    
    % Setup perspective projection component
    
            moglcore('glDisable',GL.TEXTURE_2D);
            moglcore('glEnable',GL.LIGHTING);
            moglcore('glEnable',GL.LIGHT0);
    

        % set up the projection
     moglcore('glMatrixMode',GL.MODELVIEW);
     moglcore('glLoadIdentity');
    
   
        %rebuild the texture matrix
        %BFRebuildTextureMatrix('projection');
    
     
   
        % Clear out the backbuffer: This also cleans the depth-buffer for
        % proper occlusion handling:
        moglcore('glClear', mor(GL.COLOR_BUFFER_BIT,GL.DEPTH_BUFFER_BIT));
        
               
            moglcore('glMatrixMode',GL.PROJECTION);
    moglcore('glLoadIdentity');
  
    
   
    %glFrustum(leftw,  rightw,  bottomw,  topw, clip_near,  clip_far);
    left=nearClip*tan((-horizFOV/2+deghorizoffset(depthplane+whichEye*4)-horizFOVoffset(depthplane+whichEye*4))*pi/180);
    right=nearClip*tan((+horizFOV/2+deghorizoffset(depthplane+whichEye*4)+horizFOVoffset(depthplane+whichEye*4))*pi/180);
    top=nearClip*tan((+vertFOV/2+degvertoffset(depthplane+whichEye*4)+vertFOVoffset(depthplane+whichEye*4))*pi/180);
    bottom=nearClip*tan((-vertFOV/2+degvertoffset(depthplane+whichEye*4)-vertFOVoffset(depthplane+whichEye*4))*pi/180);


        moglcore('glFrustum',left, right, bottom, top, nearClip, farClip);
       
    

        %rebuild the texture matrix a second time
        %BFRebuildTextureMatrix('projection');
    
    
     moglcore('glMatrixMode',GL.MODELVIEW);
     moglcore('glLoadIdentity');
            
            
      
            
            
            %Tranlate the scene with respect to the eye
            moglcore('glTranslatef',-IPD/2+whichEye*IPD,  0,  0);
            

 
    
        
    
            moglcore( 'glPushMatrix' );
                moglcore('glMaterialfv',GL.FRONT_AND_BACK, GL.AMBIENT, matSphereAmb);
                moglcore('glMaterialfv',GL.FRONT_AND_BACK, GL.DIFFUSE, matSphereDiff);

                    % glutSolidSphere( radius, slices, stacks )
                    moglcore('glPushMatrix');
                    moglcore('glTranslatef',-.06,0,-.346);
                    moglcore('glutSolidSphere',0.015, 10, 10);
                    moglcore( 'glPopMatrix' );

                    % glutSolidSphere( radius, slices, stacks )
                    moglcore('glPushMatrix');
                    moglcore('glTranslatef',0,0,-.46);
                    moglcore('glutSolidSphere',0.02, 10, 10);
                    moglcore( 'glPopMatrix' );            

                     % glutSolidSphere( radius, slices, stacks )
                    moglcore('glPushMatrix');
                    moglcore('glTranslatef',.1,0,-.65);
                    moglcore('glutSolidSphere',0.03, 10, 10);
                    moglcore( 'glPopMatrix' );           

            moglcore( 'glPopMatrix' );
            
            
            moglcore('glPushMatrix');
                moglcore('glMaterialfv',GL.FRONT_AND_BACK, GL.AMBIENT, matCubeAmb);
                moglcore('glMaterialfv',GL.FRONT_AND_BACK, GL.DIFFUSE, matCubeDiff);
                    % glutSolidCube( size )
                    cubeSize    = .05;
                    moglcore('glPushMatrix');
                    moglcore('glTranslatef',0.07, 0.07, -.3);
                    moglcore('glutSolidCube',cubeSize);
                    moglcore( 'glPopMatrix' );           

                    % glutSolidCube( size )
                    moglcore('glPushMatrix');
                    moglcore('glTranslatef',-.07, .07, -.41);
                    moglcore('glutSolidCube',cubeSize);
                    moglcore('glPopMatrix' );  

                    % glutSolidCube( size )
                    moglcore('glPushMatrix');
                    moglcore('glTranslatef',-.07, -.07, -.55);
                    moglcore('glutSolidCube',cubeSize);
                    moglcore('glPopMatrix' );  

                    % glutSolidCube( size )
                    moglcore('glPushMatrix');
                    moglcore('glTranslatef',0.07, -.07, -.8);
                    moglcore('glutSolidCube',cubeSize);
                    moglcore('glPopMatrix' );              
    
            moglcore( 'glPopMatrix' );
      
            
    
            %I am not sure if this code is essential
                moglcore('glDisable',GL.DEPTH_TEST);
                moglcore('glDisable',GL.LIGHTING);
                moglcore('glDisable',GL.COLOR_MATERIAL);

                %Call DepthMatte(Disable)
                moglcore('glDisable',GL.TEXTURE_1D);
                moglcore('glDisable',GL.TEXTURE_GEN_S);
                moglcore('glDisable',GL.TEXTURE_GEN_T);
                moglcore('glDisable',GL.TEXTURE_GEN_R);
                moglcore('glDisable',GL.TEXTURE_GEN_Q);

        %BFRebuildTextureMatrix('texture');        
            %End of code of ambiguous value
            
            
            
            
    moglcore('glMatrixMode',GL.PROJECTION);
   
   
    % Finish OpenGL rendering into PTB window and check for OpenGL errors.
    Screen('EndOpenGL', windowPtr);

