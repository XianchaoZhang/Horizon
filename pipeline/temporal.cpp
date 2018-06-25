#include <stdio.h>
#include <string>
#include <iostream>
#include <random>
#include <math.h>
#include <algorithm>
#include <vector>
#include <list>
#include <time.h>
#include "Halide.h"
#include "halide_image_io.h"
#include "temporal.h"

using namespace std;
using namespace Halide; 
using namespace Halide::ConciseCasts;

extern const int K = 11;
const int n_frames = 5;
const int s = 10;
int sig_s;

Buffer<uint8_t> input[n_frames];
int width;
int height;
int n_channels;
Func D, I;

Buffer<short> pix(1, 1, 1, 1, 1); //x, y, x_i, y_i, c

int main(int argc, char** argv) {

	srand(17);

	get_input();
	
	// Buffer<vector<vector<short>>*> b(width, height, n_channels);
	vector<vector<short>> neighbors;
	vector<short> v_i;
	ushort coord[2];

	for(int y = 0; y < 10; y++) {
		for(int x = 0; x < 10; x++) {
			for(int i = 0; i < K; i++) {

				set_v_i(x, y, &v_i);
				// print_v_i(v_i);
				neighbors.push_back(v_i);
				clear_v_i(&v_i);
			}
			
			sort_heap_last_element(&neighbors);	
			if(x<3 && y<3) print_heap(neighbors, x, y);
			// b(x, y, 0) = neighbors;		
		}
	}
}


short* get_rand_coord(short* coord) {
	
	coord[0] = get_rand_x_y();
	coord[1] = get_rand_x_y();
	// print_coord(coord);
	return coord;
}



short get_rand_x_y() {

	float foo = sig_s * box_muller_trans((float) rand() / RAND_MAX);	
	// cout<<"get_rand_x_y() returning "<<(short)foo<<endl;
	return (short) foo;
}

void set_v_i(ushort x, ushort y, vector<short>* v_i) {
	
	short coord[2];
	get_rand_coord(coord);
	cout<<"Printing coordinate before realizing"<<endl;
	print_coord(coord);
	pix.set_min(x, y, x + coord[0], y + coord[1], 0);			
	D.realize(pix);
	
	int D_i = pix(x, y, x + coord[0], y + coord[1], 0);
	// cout<<D_i<<endl;

	v_i->push_back(coord[0]); //x
	v_i->push_back(coord[1]); //y
	v_i->push_back(D_i);
	cout<<"Printing coordinate before realizing and pushing"<<endl;
	print_coord(coord);

}

void clear_v_i(vector<short>* v_i) {
	v_i->pop_back();
	v_i->pop_back();
	v_i->pop_back();
}

// converts a uniform random variable into a standard normal variable
float box_muller_trans(float x) {
	float foo = sqrt(-2 * log(x)) * cos(2 * M_PI * x);
	// cout<<"box_muller_trans() returning "<<foo<<endl;
	return foo;
}

void get_input() {

	string path;

	for(int i = 0; i < n_frames; i++) {

		path = "./frames/f_" + to_string(i + 1) + ".jpg";
		input[i] = Tools::load_image(path);	

	}

	width = input[0].width();
	height = input[0].height();
	n_channels = input[0].channels();
	
	sig_s = width / 3;
	Var x, y, x_i, y_i, c;
	RDom u(-s, s, -s, s);

	Expr clamped_x = clamp(x, s, width - s);
	Expr clamped_y = clamp(y, s, height - s);
	
	I(x, y, c) = input[0](clamped_x, clamped_y, c);	
	D(x, y, x_i, y_i, c) = i16(sum(pow((I(x + u.x, y + u.y, c) - I(x_i + u.x, y_i + u.y, c)), 2)));
	// D.trace_stores();

}

