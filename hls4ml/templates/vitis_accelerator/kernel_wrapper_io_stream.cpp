#include "kernel_wrapper.h"
#include "firmware/myproject.h"

static void read_input(const in_buffer_t *in, hls::stream<input_t> &input, int n) {
  for (int i = 0; i < DATA_SIZE_IN; i++) {
    #pragma HLS PIPELINE
    input_t tmp;
    for (int j = 0; j < NNET_ARRAY_DEPTH; j++) {
      #pragma HLS UNROLL
      tmp[j] = in[(n * DATA_SIZE_IN * NNET_ARRAY_DEPTH) + (i * NNET_ARRAY_DEPTH) + j];
    }
    input << tmp;
  }
}

static void write_result(out_buffer_t *out, hls::stream<result_t> &output, int n) {
  result_t tmp = output.read();
  for (int i = 0; i < DATA_SIZE_OUT; i++) {
    #pragma HLS UNROLL
    out[(n * DATA_SIZE_OUT) + i] = tmp[i];
  }
}

extern "C" {
  /**
    \brief HLS4ML Kernel Implementation 
    \param in Input Vector
    \param out Output Vector
*/
  void kernel_wrapper(const in_buffer_t *in, out_buffer_t *out) {
    hls::stream<input_t> input("input");
    hls::stream<result_t> output("output");
    #pragma HLS STREAM variable=input depth=DATA_SIZE_IN
    #pragma HLS STREAM variable=output depth=1
    
    for (int n = 0; n < BATCHSIZE; n++) {
    #pragma HLS DATAFLOW          
      read_input(in, input, n);
      myproject(input, output);
      write_result(out, output, n);
    }
  }
}