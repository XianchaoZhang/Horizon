#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex>
#include "Halide.h"
#include "halide_image_io.h"
#include "denoise.h"
#define L 10
#define W 2


using namespace Halide;
using namespace Halide::ConciseCasts;
using namespace std;


int main(int argc, char **argv) {

	Buffer<uint8_t> input = Tools::load_image(argv[1]);
		
	printf("channels=%d\n", input.channels());
	float sig_s_f = atof(argv[2]);
	float sig_r_f = atof(argv[3]);
	float epsi_f  = atof(argv[4]);

	Var x, y, c, t;

	// declare range and spatial filters
	Func g_sig_s, g_sig_r;
	RDom omega(-W, W, -W, W), l_r(-L, L, -L, L);;


	g_sig_s(x, y) = f32(0);
	

	g_sig_s(omega.x, omega.y) = f32(exp(-(omega.x * omega.x + omega.y * omega.y) / (2 * sig_s_f * sig_s_f)));

	g_sig_r(t) = f32(exp(- t * t / (2 * sig_r_f * sig_r_f)));
	//g_sig_r(t) = t;

	Func imp_bi_filter, imp_bi_filter_num, imp_bi_filter_den;

	
	// Compute box filtered image
	Buffer<uint8_t> box_filtered(input.width(), input.height(), input.channels()); 

	Func box_filtered_test, foo;

	box_filtered(x, y, c) = (f32((1 / ((2 * L + 1) * (2 * L + 1))) * sum(input(x - l_r.x, y - l_r.y, c))));

	Buffer<uint8_t> box_filtered = foo.realize(input.width(), input.height(), input.channels());
	box_filtered_test(x, y, c) = print(box_filtered(x, y, c));
	imp_bi_filter_num(x, y, c) = u8(min(sum(g_sig_s(omega.x, omega.y) * g_sig_r(box_filtered(x - omega.x, y - omega.y, c) - box_filtered(x, y, c)) * input(x - omega.x, y - omega.y, c)), 255));
	imp_bi_filter_den(x, y, c) = (u8(sum(g_sig_s(omega.x, omega.y) * (g_sig_r((box_filtered(x - omega.x, y - omega.y, c) - box_filtered(x, y, c)))))));
	imp_bi_filter(x, y, c) = u8(imp_bi_filter_num(x, y, c) / imp_bi_filter_den(x, y, c));
	// // imp_bi_filter.trace_stores();

	Buffer<uint8_t> shifted(input.width() - 2 * W, input.height() - 2 * W, input.channels());
	shifted.set_min(W, W);

	box_filtered_test.realize(shifted);
	//imp_bi_filter.realize(shifted);

	Tools::save_image(shifted, "output.png");

}