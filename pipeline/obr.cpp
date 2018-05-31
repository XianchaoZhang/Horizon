// only header file we NEED is Halide.h
#include "Halide.h"
#include<stdio.h>
// include support for loading pngs
#include "halide_image_io.h"
#define OBR_HEIGHT (uint8_t) 30
using namespace Halide;

int main(int argc, char **argv) { 

    /***************From lesson 1****************/

    // Load the input image that we want to correct the obr offset for
    Buffer<uint8_t> input = Tools::load_image(argv[1]);

    //define the obrOffset function. OBR stands for Optical Black Region.
    Func obrOffset;


    // x, y - position. c - color channel
    Var x, y, c;


    // for each pixel of the input image
    Expr value = input(x, y, c);
    	
    // cast it to a floating point value.	
    value = cast<float>(value);

    /*******************************************/
    
    /***************From lesson 7***************/ 

    /* Note: Lesson 3 states that "unfortunately in C++, objects don't know their own names, which makes it 
    hard for us to understand the generated code. To get around this, you can pass a string to the Func and 
    Var constructors to give them a name for debugging purposes." */
    // Func clamped("clamped");
    
    /* taking only the OBR region, specified in the main function arguments. */
    Expr clamped_x = clamp(x, 0, input.width());
    Expr clamped_y = clamp(y, 0, 10);

    
    // tell Halide that we'd like to be notified of all evaluations.
    //clamped.trace_stores();
   
    Func clamped("clamped");
    clamped(x, y, c) = input(clamped_x, clamped_y, c);

    // input_16 is the OBR region cast to 16 bit for calculations so that it doesn't overflow 
    Func input_16("input_16");
    //input_16(x, y, c) = cast<uint16_t>(clamped(x, y, c));     
    // input_16 = cast<float>(input_16);
    input_16(x, y, c) = cast<uint16_t>(input(x, y, c));    
    /******************************************/ 

    // defining obrOffset as the (x, y, c) value minus the average from the obr region.

    RDom r(0, input.width(), 0, 10);
    
    Expr average = sum(input(r.x, r.y, c))/(OBR_HEIGHT * cast<uint16_t>(input.width()));
    obrOffset(x, y, c) = cast<uint8_t>(max((input_16(x, y, c)) - average, 0));

    
    Buffer<uint8_t> result = obrOffset.realize(input.width(), input.height(), input.channels());

    // Save the output for inspection..
    Tools::save_image(result, "result.png");
   
    printf("Success!\n");
    return 0;

        
}

