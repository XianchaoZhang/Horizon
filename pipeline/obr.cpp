// only header file we NEED is Halide.h
#include "Halide.h"
#include<stdio.h>
// include support for loading pngs
#include "halide_image_io.h"
#define OBR_HEIGHT 10
using namespace Halide; 
using namespace Halide::ConciseCasts;

int main(int argc, char **argv) { 

 

 
    Buffer<uint8_t> input = Tools::load_image(argv[1]);

    
    Func obrOffset;


    Var x, y, c;


    Expr value = f32(input(x, y, c));

    // input_16 is the OBR region cast to 16 bit for calculations so that it doesn't overflow 
    Func input_16("input_16");

    input_16(x, y, c) = u16(input(x, y, c));    

    Func black;
    black(x, y, c) = 100;
    
    
    RDom r(0, input.width(), 0, 10);

    
    Expr average = sum(black(r.x, r.y, c))/(OBR_HEIGHT * u16(input.width()));
    
    obrOffset(x, y, c) = u8(max((input_16(x, y, c)) - average, 0));

    
    Buffer<uint8_t> result = obrOffset.realize(input.width(), input.height(), input.channels());

    Tools::save_image(result, "result.png");
   
    printf("Success!\n");
    return 0;

        
}