#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define L 1
#define W 1

using namespace Halide;
using namespace ConciseCasts;

int main(int argc, char** argv) {
	Buffer<uint8_t> input = Tools::load_image(argv[1]);

	
}