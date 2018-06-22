#include <stdio.h>
#include <string>
#include <iostream>
#include "Halide.h"
#include "halide_image_io.h"

using namespace std;
using namespace Halide; 
using namespace Halide::ConciseCasts;


int main(int argc, char** argv) {
	
	Buffer<int> b(1);
	//b.set_min(10, 30);

	Func F;
	Var x, y;
	F(x, y) = x + y;

	//F.realize(b);
	b(6, 8, 1) = 10;
	b(2, 3, 2) = 6;
	cout<<b(6, 8, 1)<<endl;
	cout<<b(2, 3, 2)<<endl;
	

}

