#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Halide.h"
#include "halide_image_io.h"
#include "denoise.h"
#include <complex>
#define L 1
#define W 1

using namespace Halide;
using namespace Halide::ConciseCasts;


#define L 1
#define W 1

using namespace std;
using namespace Halide;
using namespace ConciseCasts;

int main(int argc, char** argv) {
	Buffer<uint8_t> input = Tools::load_image(argv[1]);

	

	
}