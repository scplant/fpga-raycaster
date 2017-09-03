#include "toplevel.h"
#define NUMDATA 480000
#define NUMSPHERES 20

u32 mainmemory[NUMDATA];
void get_sphere(u32 *spheres, int n, float x, float y, float z, float radius, int r, int g, int b);

int main() {

    //Create input data
    for(int i = 0; i < NUMDATA; i++) {
        mainmemory[i] = 0;
    }

    get_sphere(mainmemory, 0, -4, -3.5, 0.9, 1.6, 14, 241, 90);
    get_sphere(mainmemory, 1, 1.4, -3.3, 1.1, 1.1, 65, 146, 111);
    get_sphere(mainmemory, 2, 2.9, -3.1, -1.0, 1.6, 89, 168, 137);
    get_sphere(mainmemory, 3, 0.6, 4.1, -4.6, 0.7, 0, 22, 250);
    get_sphere(mainmemory, 4, 3.3, -3.7, -2.3, 1.5, 136, 3, 130);
    get_sphere(mainmemory, 5, -4.4, 3.1, -0.9, 1.1, 217, 241, 145);
    get_sphere(mainmemory, 6, 0, -1.8, -2.3, 1.3, 170, 17, 188);
    get_sphere(mainmemory, 7, -0.2, 3.5, -4, 1, 26, 139, 29);
    get_sphere(mainmemory, 8, 4.7, 1.5, 0.2, 1.4, 209, 249, 235);
    get_sphere(mainmemory, 9, -4.8, 1.2, -3.4, 0.8, 124, 34, 207);
    get_sphere(mainmemory, 10, 2.3, -0.2, 1.9, 1.9, 240, 42, 58);
    get_sphere(mainmemory, 11, 3.5, 4.3, -1.2, 1.5, 10, 145, 125);
    get_sphere(mainmemory, 12, 1.4, 1.4, 4, 0.8, 255, 255, 0);
    get_sphere(mainmemory, 13, -0.6, -1.5, 3.3, 0.8, 0, 128, 228);
    get_sphere(mainmemory, 14, -1.5, -3.8, -1.9, 0.8, 19, 116, 205);
    get_sphere(mainmemory, 15, -3.8, -4.4, -3.8, 1.4, 45, 34, 150);
    get_sphere(mainmemory, 16, -1.6, -0.5, -4.3, 1, 113, 110, 103);
    get_sphere(mainmemory, 17, 0.4, -0.3, 4.9, 0.9, 11, 30, 13);
    get_sphere(mainmemory, 18, -0.3, -1.1, 4.3, 0.6, 55, 29, 75);
    get_sphere(mainmemory, 19, -0.7, 4.2, -0.6, 1.5, 121, 235, 210);

//    mainmemory[0] = 960; //-4(pos+100) *10
//    mainmemory[1] = 965;
//    mainmemory[2] = 1009;
//
//    mainmemory[3] = 16; //radius *10
//
//    mainmemory[4] = 14;
//    mainmemory[5] = 241;
//    mainmemory[6] = 90;
    //Set up the slave inputs to the hardware

//    vec_t center = {-4, -3.5, 0.9};
//
//    col_t col = {14, 241, 90};
//
//    sphere_t sphere;
//    sphere.center = center;
//    sphere.radius = 1.6;
//    sphere.col    = col;

    u32 num_spheres = 20;

    //Run the hardware
    u32 return_Val = toplevel(mainmemory);


    //Following code to generate a bmp adapted from.. http://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries

    int w = 800;
    int h= 600;

    int red[w][h];
    int green[w][h];
    int blue[w][h];

    for(int i = 0; i < NUMDATA ; i++) {
    	int y = i/w;
    	int x = i - y*w;
    	green[x][y] = 0;
    	red[x][y] = 0;
    	blue[x][y] = 0;

        if(mainmemory[i] != 0){
        	green[x][y] = mainmemory[i] & 0xFF;
        	red[x][y] = (mainmemory[i]>>16) & 0xFF;
        	blue[x][y] = (mainmemory[i]>>8) & 0xFF;
        }
    }

    FILE *f;
    unsigned char *img = NULL;
    int filesize = 54 + 3*w*h;
    if( img )
        free( img );
    img = (unsigned char *)malloc(3*w*h);
    memset(img,0,sizeof(img));

    int x;
    int y;
    int r;
    int g;
    int b;
    for(int i=0; i<w; i++)
    {
        for(int j=0; j<h; j++)
    {
        x=i; y=(h-1)-j;
        r = red[i][j];
        g = green[i][j];
        b = blue[i][j];
        if (r > 255) r=255;
        if (g > 255) g=255;
        if (b > 255) b=255;
        img[(x+y*w)*3+2] = (unsigned char)(r);
        img[(x+y*w)*3+1] = (unsigned char)(g);
        img[(x+y*w)*3+0] = (unsigned char)(b);
    }
    }

    unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
    unsigned char bmppad[3] = {0,0,0};

    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);

    bmpinfoheader[ 4] = (unsigned char)(       w    );
    bmpinfoheader[ 5] = (unsigned char)(       w>> 8);
    bmpinfoheader[ 6] = (unsigned char)(       w>>16);
    bmpinfoheader[ 7] = (unsigned char)(       w>>24);
    bmpinfoheader[ 8] = (unsigned char)(       h    );
    bmpinfoheader[ 9] = (unsigned char)(       h>> 8);
    bmpinfoheader[10] = (unsigned char)(       h>>16);
    bmpinfoheader[11] = (unsigned char)(       h>>24);

    f = fopen("testimg.bmp","wb");
    fwrite(bmpfileheader,1,14,f);
    fwrite(bmpinfoheader,1,40,f);
    for(int i=0; i<h; i++)
    {
        fwrite(img+(w*(h-i-1)*3),3,w,f);
        fwrite(bmppad,1,(4-(w*3)%4)%4,f);
    }

    fclose(f);
}

void get_sphere(u32 *spheres, int n, float x, float y, float z, float radius, int r, int g, int b)
{
	z = z + 20;
	spheres[(5*n)] = (u32)((x*2 + 100)*10);
	spheres[(5*n)+1] = (u32)((y*2 + 100)*10);
	spheres[(5*n)+2] = (u32)(((z*2) + 100)*10); //represent pos 0,0,-10

	spheres[(5*n)+3] = (((u32) r) << 16) | (((u32) b) << 8) | ((u32) g); //col

	spheres[(5*n)+4] = (u32)(((x*x*100 + y*y*100 + z*z*100) - (radius*radius*100))*4)+10000; //4C
}
