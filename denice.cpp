#include <stdio.h>
#include <stdlib>
#include <math.h>
#include <complex>

#define L 1
#define W 1

using namespace std;
using namespace Halide;
using namespace ConciseCasts;

int main(int argc, char** argv) {
	complex<double> num = polar(5, 50);
	cout<< "num = "<< num << endl;
	Buffer<uint8_t> input = Tools::load_image(argv[1]);

	

	
}