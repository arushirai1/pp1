#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <complex.h>
#include <pthread.h>

#include "mandelbrot_set.h"

void *img;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct args_mandlebot {
	int x_resolution;
	int y_resolution;
	int max_iter;
    double view_x0;
    double view_x1;
    double view_y0;
    double view_y1;
    double x_stepsize;
    double y_stepsize;
    int palette_shift;
    //char *(*image);
    int num_threads;
    int i;
};

void* draw(void *args) {
	struct args_mandlebot *arg = (struct args_mandlebot*)args;
	double y;
	double x;

	complex double Z;
	complex double C;

	int k;
	printf("%d\n", arg->i);
	for(int i = arg->i; i < arg->y_resolution && i < (arg->i + arg->y_resolution/4); i++) {
		//printf("%d\n", i);

		for (int j = 0; j < arg->x_resolution; j++) {
				pthread_mutex_lock(&mutex);

				unsigned char (*img1)[arg->x_resolution][3] = img;
				pthread_mutex_unlock(&mutex);
				y = arg->view_y1 - i * arg->y_stepsize;
				x = arg->view_x0 + j * arg->x_stepsize;

				Z = 0 + 0 * I;
				C = x + y * I;

				k = 0;

				do
				{
					Z = Z * Z + C;
					k++;
				} while (cabs(Z) < 2 && k < arg->max_iter);

				if (k == arg->max_iter)
				{
					pthread_mutex_lock(&mutex);
					memcpy(img1[i][j], "\0\0\0", 3);
					pthread_mutex_unlock(&mutex);
				}
				else
				{
					int index = (k + arg->palette_shift)
					            % (sizeof(colors) / sizeof(colors[0]));
					pthread_mutex_lock(&mutex);
					memcpy(img1[i][j], colors[index], 3);
					pthread_mutex_unlock(&mutex);
				}
			}
		}
	}

void mandelbrot_draw(int x_resolution, int y_resolution, int max_iter,
	                double view_x0, double view_x1, double view_y0, double view_y1,
	                double x_stepsize, double y_stepsize,
	                int palette_shift, unsigned char (*image)[x_resolution][3],
						 int num_threads) {


	int rem = y_resolution / 4;
	struct args_mandlebot arg_threads[4];
	img = (void *)image;
	pthread_t *threads = ( pthread_t *) malloc ( 4 * sizeof ( pthread_t ) );
	for (int i = 0; i < 4; i++)
	{
		arg_threads[i].x_resolution = x_resolution;
		arg_threads[i].y_resolution = y_resolution;
		arg_threads[i].max_iter = max_iter;
		arg_threads[i].view_x0 = view_x0;
		arg_threads[i].view_x1 = view_x1;
		arg_threads[i].view_y0 = view_y0;
		arg_threads[i].view_y1 = view_y1;
		arg_threads[i].x_stepsize = x_stepsize;
		arg_threads[i].y_stepsize = y_stepsize;
		arg_threads[i].palette_shift = palette_shift;

		arg_threads[i].num_threads = num_threads;
		pthread_mutex_lock(&mutex);
		arg_threads[i].i = i*(rem);
		printf("%s:%d\n",  "in creation main thread", arg_threads[i].i);
		//draw(&args);
		pthread_create(&threads[i], NULL, draw, &arg_threads[i]);
		pthread_mutex_unlock(&mutex);
	}

	for (int i = 0; i < 4; i++)
	{
		pthread_join(threads[i], NULL);
	}

	free(threads);
	// TODO:
	// implement your solution in this file.
}
