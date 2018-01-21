#ifdef _MSC_VER
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif /* _MSC_VER */

#include <stdio.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

#define SCALE_BITS      11
#define SCALE           (1<<SCALE_BITS)

void sdl_draw_mandelbrot(SDL_Surface *surface)
{
    short x,y;                      // pixel coordinates

    // mandelbrot coordinate space is fixed point, the decimal point is SCALE_BITS from LSB
    short a,b;                      // coordinate in mandlebrot space
    short min_y = -1.0*SCALE;       // top edge in mandlebrot space
    short min_x = -2*SCALE;         // left edge in mandlebrot space
    short inc = 3*SCALE/WIDTH;      // pixel size

    short n, maxiter = 0xff;        // iteration counter
    short z_re, z_im;
    short z_re2, z_im2;     // intermediate squared terms

    fprintf(stderr, "inc: %d\n", inc);
    fprintf(stderr, "min_y: %d\n", min_y);
    fprintf(stderr, "min_x: %d\n", min_x);

    int totaliter = 0;
    int lineiter = 0;
    int min_iter = 0;
    int max_iter = 0;

    for (y = 0, b = min_y; y < HEIGHT; y++, b+=inc)
    {
        lineiter = 0;
        for (x = 0, a = min_x; x < WIDTH; x++, a+=inc)
        {
            // initial conditions for iteration
            z_re = a;
            z_im = b;

            for (n = 0; n <= maxiter; n++)
            {
                totaliter++; lineiter++;
                z_re2 = (z_re * z_re)>>SCALE_BITS;
                z_im2 = (z_im * z_im)>>SCALE_BITS;

                if(z_re2 + z_im2 > 4<<SCALE_BITS) {
                    break;
                }

                z_im = (2*(z_re*z_im/SCALE) + b);
                z_re = (z_re2 - z_im2 + a);
            }

            // iteration count -> RGB888
            int red = (~n & 0xE0);
            int green = (n & 0x1C) << 3;
            int blue = (n & 0x03) << 5;

            ((Uint32*)surface->pixels)[(y*surface->w) + x] = (n >= maxiter)? 0 :
                SDL_MapRGB( surface->format, red, green, blue );

        }
        if(lineiter < min_iter || min_iter == 0) min_iter = lineiter;
        if(lineiter > max_iter) max_iter = lineiter;
        //fprintf(stderr, "line %d iter: %d\n", y+1, lineiter);
    }

    SDL_UpdateRect(surface, 0, 0, 0, 0);
    fprintf(stderr, "total iterations: %d\n", totaliter);
    fprintf(stderr, "max line iter: %d\n", max_iter);
    fprintf(stderr, "min line iter: %d\n", min_iter);
}

int main(int argc, char **argv)
{
    /* SDL SEtup */
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    atexit(SDL_Quit);

    SDL_WM_SetCaption("Mandelbrot", "Mandelbrot");

    SDL_Surface *surface;

    surface = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE);
    if ( surface == NULL )
    {
        fprintf(stderr, "Could not setup screen to resolution %dx%d : %s\n", 
                WIDTH, HEIGHT, SDL_GetError());
        exit(1);
    }

    SDL_Event event;

    while(1)
    {
        SDL_PollEvent(&event);
        switch (event.type)
        {
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    exit(0);
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                sdl_draw_mandelbrot(surface);
                break;
        }
    }

    return 0;
}
