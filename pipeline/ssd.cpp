#include <stdio.h>
#include <string>
#include <iostream>
#include "Halide.h"
#include "halide_image_io.h"

using namespace std;
using namespace Halide; 
using namespace Halide::ConciseCasts;


void do_stuff(Buffer<short> b, int i) {
	b(i, i, i, i, i) = i + 1;
}


int main(int argc, char** argv) {
	


	int width = 10;
	int height = 10;
	int n_channels = 3;
	Buffer<short> b(width); //x, y, x_i, y_i, channels. 


	for(int i = 0; i < height; i++) {
	
		b(i, i, i, i, i) = i;
	
	}
	cout<<"before"<<endl;
	for(int i = 0; i < 10; i++) {
		cout<<b(i, i, i, i, i)<<endl;
	
	}
	
	for(int i = 0; i < 10; i++) {
		do_stuff(b, i);
	}
	
	cout<<"after"<<endl;
	for(int i = 0; i < 10; i++) {
		cout<<b(i, i, i, i, i)<<endl;
	}
}

