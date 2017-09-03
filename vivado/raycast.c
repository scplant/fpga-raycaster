#include "raycast.h"
#include "timing.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <xgpio.h>

#include <stdio.h>
#include "xil_types.h"
#include "xil_cache.h"
#include "xtoplevel.h"
#include "platform.h"
#include "xparameters.h"
#include "zybo_vga/display_ctrl.h"
#include "xtime_l.h"

// Frame size
#define MAX_FRAME (800*600)
#define FRAME_STRIDE (800*4)

XToplevel hls; //init IP core

DisplayCtrl dispCtrl; // Display driver struct
u32 buff;
u32 frameBuf[DISPLAY_NUM_FRAMES][MAX_FRAME]; // Frame buffers for video data
void *pFrames[DISPLAY_NUM_FRAMES]; // Array of pointers to the frame buffers

void get_spheres(u32 *spheres, int *intspheres, bool is_random);
void get_sphere(u32 *spheres, int *intspheres, int n, float x, float y, float z, float radius, int r, int g, int b);

int main(void)
{
    init_platform();

    XGpio io; // Initialise buttons
    XGpio_Initialize(&io, XPAR_GPIO_0_DEVICE_ID);
    XGpio_SetDataDirection(&io, 1, 0xF);
    XGpio_SetDataDirection(&io, 2, 0xF);

    XToplevel_Initialize(&hls, XPAR_TOPLEVEL_0_DEVICE_ID);

    // Initialise an array of pointers to the 2 frame buffers
    	int i;
    	for (i = 0; i < DISPLAY_NUM_FRAMES; i++)
    		pFrames[i] = frameBuf[i];

    	// Initialise the display controller
    	DisplayInitialize(&dispCtrl, XPAR_AXIVDMA_0_DEVICE_ID, XPAR_VTC_0_DEVICE_ID, XPAR_VGA_AXI_DYNCLK_0_BASEADDR, pFrames, FRAME_STRIDE);

    	// Use first frame buffer (of two)
    	DisplayChangeFrame(&dispCtrl, 0);

    	// Set the display resolution
    	DisplaySetMode(&dispCtrl, &VMODE_800x600);

    	// Enable video output
    	DisplayStart(&dispCtrl);

    	printf("VGA output enabled\n\r");
    	printf("Current Resolution: %s\n\r", dispCtrl.vMode.label);
    	printf("Pixel Clock Freq. (MHz): %.3f\n\r", dispCtrl.pxlFreq);

    	// Get parameters from display controller struct
    	//u32 width  = dispCtrl.vMode.width;
    	//u32 height = dispCtrl.vMode.height;
    	u32 *frame = dispCtrl.framePtr[dispCtrl.curFrame];
    	buff   = dispCtrl.curFrame;
    	memset(frame, 0xFF, MAX_FRAME*4);
    	while(1) { //main program
    		int num_spheres = 20;
			bool is_random = false;
			char reply = 'n';
			printf("Random spheres? y/n\n"); // Ask the user if they would like to use random spheres.
			scanf(" %c", &reply);
			printf("%c\n", reply);
			if (reply == 'y') {
				is_random = true;
				// If random ask the user to specify number of spheres
			}

			printf("How many spheres?\n");
			scanf(" %d", &num_spheres);
			printf("%d\n", num_spheres);

			u32 spheres[20*5]; // x y z col C in unsigned converted form
			int intspheres[20*4]; // x y z r in int form
			get_spheres(spheres, intspheres, is_random);
			if (num_spheres < 20) //set unwanted spheres to 0
			{
				for(i=num_spheres*5;i<20*5;i++){
					spheres[i] = 0;
				}
			}
			double angle;
			angle = 0.1;

			u32 val;
			u32 switch_axis;
			while (1) {
				// Switch the frame we're modifying to be back buffer (1 to 0, or 0 to 1)
				buff = !buff;
				frame = dispCtrl.framePtr[buff];

				memcpy(frame, spheres, 4*5*20);

				XToplevel_Set_ram(&hls, (int) frame);
				XToplevel_Start(&hls); //start accelerator

				switch_axis = XGpio_DiscreteRead(&io, 2);
				val = XGpio_DiscreteRead(&io, 1);
				if (switch_axis & 8) {
					//Button inputs
					if (val & 8) {
						for (i=0; i < num_spheres; i++) {

							intspheres[i*4+0] += 1; //inc sphere x

							int x = (intspheres[i*4+0]);
							int y = (intspheres[i*4+1]);
							int z = (intspheres[i*4+2]);
							int r = (intspheres[i*4+3]);
							spheres[i*5] = (x*2+1000);
							int C = (((x*x + y*y + z*z) - (r*r))*4) +10000; //recalculate C
							spheres[i*5 + 4] = C;
						}
					}
					if (val & 4) {
						for (i=0; i < num_spheres; i++) {
							intspheres[i*4+0] -= 1; //dec sphere x

							int x = (intspheres[i*4+0]);
							int y = (intspheres[i*4+1]);
							int z = (intspheres[i*4+2]);
							int r = (intspheres[i*4+3]);
							spheres[i*5] = (x*2+1000);
							int C = (((x*x + y*y + z*z) - (r*r))*4) +10000; //recalculate C
							spheres[i*5 + 4] = C;
						}
					}
					if (val & 2) {
						for (i=0; i < num_spheres; i++) {
							intspheres[i*4+1] += 1; //inc sphere y

							int x = (intspheres[i*4+0]);
							int y = (intspheres[i*4+1]);
							int z = (intspheres[i*4+2]);
							int r = (intspheres[i*4+3]);
							spheres[i*5+1] = (y*2+1000);
							int C = (((x*x + y*y + z*z) - (r*r))*4) +10000; //recalculate C
							spheres[i*5 + 4] = C;
						}
					}
					if (val & 1) {
						for (i=0; i < num_spheres; i++) {
							intspheres[i*4+1] -= 1; //dec sphere y

							int x = (intspheres[i*4+0]);
							int y = (intspheres[i*4+1]);
							int z = (intspheres[i*4+2]);
							int r = (intspheres[i*4+3]);
							spheres[i*5+1] = (y*2+1000);
							int C = (((x*x + y*y + z*z) - (r*r))*4) +10000; //recalculate C
							spheres[i*5 + 4] = C;
						}
					}
				} else {
					if (val & 8) {
						for (i=0; i < num_spheres; i++) {
							intspheres[i*4+2] += 1; //inc sphere z

							int x = (intspheres[i*4+0]);
							int y = (intspheres[i*4+1]);
							int z = (intspheres[i*4+2]);
							int r = (intspheres[i*4+3]);
							spheres[i*5+2] = (z*2+1000);
							int C = (((x*x + y*y + z*z) - (r*r))*4) +10000; //recalculate C
							spheres[i*5 + 4] = C;
						}
					}
					if (val & 4) {
						for (i=0; i < num_spheres; i++) {
							intspheres[i*4+2] -= 1; //dec sphere z

							int x = (intspheres[i*4+0]);
							int y = (intspheres[i*4+1]);
							int z = (intspheres[i*4+2]);
							int r = (intspheres[i*4+3]);
							spheres[i*5+2] = (z*2+1000);
							int C = (((x*x + y*y + z*z) - (r*r))*4) +10000; //recalculate C
							spheres[i*5 + 4] = C;
						}
					}
					if (val & 2) {
						for (i=0; i < num_spheres; i++) { //updates angles
							int x = (intspheres[i*4+0]);
							int y = (intspheres[i*4+1]);
							int z = (intspheres[i*4+2]);
							int r = (intspheres[i*4+3]);
							intspheres[i*4+0] = round((z*sin(angle) + x*cos(angle)));  // x rotation z*sin q + x*cos q
							intspheres[i*4+2] = round((z*cos(angle) - x*sin(angle)));  // z rotation z*cos q - x*sin q
							x = (intspheres[i*4+0]);
							z = (intspheres[i*4+2]);

							spheres[i*5+0] = (x*2+1000);
							spheres[i*5+2] = (z*2+1000);
							int C = (((x*x + y*y + z*z) - (r*r))*4) +10000; //recalculate C
							spheres[i*5 + 4] = C;
						}
					}
					if (val & 1) {
						for (i=0; i < num_spheres; i++) { //updates angles
							int x = (intspheres[i*4+0]);
							int y = (intspheres[i*4+1]);
							int z = (intspheres[i*4+2]);
							int r = (intspheres[i*4+3]);
							intspheres[i*4+0] = round(z*sin(-angle) + x*cos(-angle));  // x rotation z*sin q + x*cos q
							intspheres[i*4+2] = round((z*cos(-angle) - x*sin(-angle)));  // z rotation z*cos q - x*sin q
							x = (intspheres[i*4+0]);
							z = (intspheres[i*4+2]);

							spheres[i*5+0] = (x*2+1000);
							spheres[i*5+2] = (z*2+1000);
							int C = (((x*x + y*y + z*z) - (r*r))*4) +10000; //recalculate C
							spheres[i*5 + 4] = C;
						}
					}
				}
				if (switch_axis & 4) { // extension greyscale mode
					for (i=0; i < num_spheres; i++) {
						u32 col = spheres[i*5+3];
						int grey = ((col & 0xFF) + ((col>>16) & 0xFF) + ((col>>8) & 0xFF))/3;
						spheres[i*5+3] = (((u32) grey) << 16) | (((u32) grey) << 8) | ((u32) grey);
					}
				}

				if (switch_axis & 2) { // extension disco mode / randomise colors
					for (i=0; i < num_spheres; i++) {
						spheres[i*5+3] = (((u32) rand() % 256) << 16) | (((u32) rand() % 256 ) << 8) | ((u32) rand() % 256 );
					}
				}

				if (switch_axis & 1) { // Change scene config
					if (val & 1 && num_spheres < 20) { //Raise number of spheres
						num_spheres += 1;
					}
					if (val & 2 && num_spheres > 0) { //Lower number of spheres
						num_spheres -= 1;
					}
					if (val & 8) { // Toggle random
						is_random = !is_random;
					}
					get_spheres(spheres, intspheres, is_random);
					if (num_spheres < 20) //set unwanted spheres to 0
					{
						for(i=num_spheres*5;i<20*5;i++){
							spheres[i] = 0;
						}
					}
				}

				if (switch_axis == 15) { // If all switches on reset and re-take user params
					break;
				}

				printf("%.3ffps\n", get_fps()); //print fps

				frame_timer();
				while(!XToplevel_IsDone(&hls));


				// Flush everything out to DDR
				Xil_DCacheFlush();
				// Switch active frame to the back buffer
				DisplayChangeFrame(&dispCtrl, buff);
				// Wait for the frame to switch (after active frame is drawn) before continuing (not needed enough delay)
				//DisplayWaitForSync(&dispCtrl);
			}
    	}


    cleanup_platform();
    return 0;
}

/*
 * get and set the spheres in the scene randomly or from the pre-defined values.
 */
void get_spheres(u32 *spheres, int *intspheres, bool is_random)
{
	if (is_random) {
		XTime ran_seed;
		XTime_GetTime(&ran_seed);
		srand(ran_seed);
		int i;
		for (i = 0; i < 20; i += 1) {

			int x = rand() % 101; // Get random xyz origin coordinates between -5.0 and 5.0
			x += -50;
			int y = rand() % 101;
			y += -50;
			int z = rand() % 101;
			z += -50;

			int r = rand() % 16; // Get a random radius between 0.5 and 2.0
			r += 5;

			int red = rand() % 256 ; // randomise color between 0 and 255
			int green = rand() % 256 ;
			int blue = rand() % 256 ;

			get_sphere(spheres, intspheres, i, (float)x/10, (float)y/10, (float)z/10, (float)r/10, red, green, blue);
		}
	}
	else {
		get_sphere(spheres, intspheres, 0, -4, -3.5, 0.9, 1.6, 14, 241, 90);
		get_sphere(spheres, intspheres, 1, 1.4, -3.3, 1.1, 1.1, 65, 146, 111);
		get_sphere(spheres, intspheres, 2, 2.9, -3.1, -1.0, 1.6, 89, 168, 137);
		get_sphere(spheres, intspheres, 3, 0.6, 4.1, -4.6, 0.7, 0, 22, 250);
		get_sphere(spheres, intspheres, 4, 3.3, -3.7, -2.3, 1.5, 136, 3, 130);
		get_sphere(spheres, intspheres, 5, -4.4, 3.1, -0.9, 1.1, 217, 241, 145);
		get_sphere(spheres, intspheres, 6, 0, -1.8, -2.3, 1.3, 170, 17, 188);
		get_sphere(spheres, intspheres, 7, -0.2, 3.5, -4, 1, 26, 139, 29);
		get_sphere(spheres, intspheres, 8, 4.7, 1.5, 0.2, 1.4, 209, 249, 235);
		get_sphere(spheres, intspheres, 9, -4.8, 1.2, -3.4, 0.8, 124, 34, 207);
		get_sphere(spheres, intspheres, 10, 2.3, -0.2, 1.9, 1.9, 240, 42, 58);
		get_sphere(spheres, intspheres, 11, 3.5, 4.3, -1.2, 1.5, 10, 145, 125);
		get_sphere(spheres, intspheres, 12, 1.4, 1.4, 4, 0.8, 255, 255, 0);
		get_sphere(spheres, intspheres, 13, -0.6, -1.5, 3.3, 0.8, 0, 128, 228);
		get_sphere(spheres, intspheres, 14, -1.5, -3.8, -1.9, 0.8, 19, 116, 205);
		get_sphere(spheres, intspheres, 15, -3.8, -4.4, -3.8, 1.4, 45, 34, 150);
		get_sphere(spheres, intspheres, 16, -1.6, -0.5, -4.3, 1, 113, 110, 103);
		get_sphere(spheres, intspheres, 17, 0.4, -0.3, 4.9, 0.9, 11, 30, 13);
		get_sphere(spheres, intspheres, 18, -0.3, -1.1, 4.3, 0.6, 55, 29, 75);
		get_sphere(spheres, intspheres, 19, -0.7, 4.2, -0.6, 1.5, 121, 235, 210);
	}
}

/*
 * Given its attributes define and return a sphere.
 */
void get_sphere(u32 *spheres, int *intspheres , int n, float x, float y, float z, float radius, int r, int g, int b)
{
	z = z + 20; // to Initialise the virtual camera position
	spheres[(5*n)]   = (u32)((x*2 + 100)*10); //*2 to optimise 2*B in intersect.
	spheres[(5*n)+1] = (u32)((y*2 + 100)*10);
	spheres[(5*n)+2] = (u32)(((z*2) + 100)*10); //represent pos 0,0,-10

	spheres[(5*n)+3] = (((u32) r) << 16) | (((u32) b) << 8) | ((u32) g); //col

	spheres[(5*n)+4] = (u32)(((x*x*100 + y*y*100 + z*z*100) - (radius*radius*100))*4)+10000; //C

	intspheres[4*n]     = (x*10);
	intspheres[(4*n)+1] = (y*10);
	intspheres[(4*n)+2] = (z*10);
	intspheres[(4*n)+3] = (radius*10);
}
