#include <iostream>

#include "firmware/myproject.h"
#include "firmware/parameters.h"
#include "firmware/myproject.cpp"

#define c_size 1024

static void load_input(input_t* in, hls::stream<input_t>& inStream, int size) {
mem_rd:
    for (int i = 0; i < size; i++) {
    #pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size   
    inStream << in[i];
    }
}
//static void store_result(result_t* out, hls::stream<result_t>& out_stream, int size) {
static void store_result(result_t* out, hls::stream<result_t>& out_stream, int size) {
mem_wr:
    for (int i = 0; i < size; i++) {
    #pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        result_t temp = out_stream.read();
        out[i] = temp;
    }
}

void myproject_kernel(
    input_t *input_1,
    result_t *layer9_out,
    uint32_t size
){
    #pragma HLS INTERFACE m_axi port = input_1 bundle = gmem0
    #pragma HLS INTERFACE m_axi port = layer9_out bundle = gmem1
    static hls::stream<input_t> input_1_stream("input_1_stream");
    static hls::stream<result_t> layer9_out_stream("layer9_out_stream");
    #pragma HLS dataflow
    load_input(input_1, input_1_stream, size);
    myproject(input_1_stream, layer9_out_stream);
    store_result(layer9_out, layer9_out_stream, size);
}