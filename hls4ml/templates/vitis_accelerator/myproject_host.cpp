/**
 * Copyright (C) 2019-2022 Xilinx, Inc
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You may
 * not use this file except in compliance with the License. A copy of the
 * License is located at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

#include <cstring>
#include <iostream>

// XRT includes
#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

////////////////// HLS4ML Includes start //////////////////

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "firmware/myproject.h"
#include "firmware/nnet_utils/nnet_helpers.h"

// hls-fpga-machine-learning insert bram

#define CHECKPOINT 5000

namespace nnet {
bool trace_enabled = true;
std::map<std::string, void *> *trace_outputs = NULL;
size_t trace_type_size = sizeof(double);
} // namespace nnet

////////////////// HLS4ML Includes end //////////////////

#define DATA_SIZE 1

int main(int argc, char** argv) {

    // Read settings
    std::string binaryFile = argv[1];
    int device_index = 0;

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Open the device" << device_index << std::endl;
    auto device = xrt::device(device_index);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    size_t vector_size_bytes_in = sizeof(input_t) * DATA_SIZE;
    size_t vector_size_bytes_out = sizeof(result_t) * DATA_SIZE;

    auto krnl = xrt::kernel(device, uuid, "myproject_kernel");

    std::cout << "Allocate Buffer in Global Memory\n";
    auto bo0 = xrt::bo(device, vector_size_bytes_in, krnl.group_id(0));
    auto bo_out = xrt::bo(device, vector_size_bytes_out, krnl.group_id(1));

    // Map the contents of the buffer object into host memory
    auto bo0_map = bo0.map<input_t*>();
    auto bo0_out_map = bo_out.map<result_t*>();
    memset((char*)bo0_map, 0, vector_size_bytes_in);
    memset((char*)bo0_out_map, 0 ,vector_size_bytes_out);

    // Create the test data
/////////////////////////// From HLS4ML test start ///////////////////////////

    // load input data from text file
    std::ifstream fin("output_dir/tb_data/tb_input_features.dat");
    // load predictions from text file
    std::ifstream fpr("output_dir/tb_data/tb_output_predictions.dat");

    std::string RESULTS_LOG = "output_dir/tb_data/hw_results.log";
    std::ofstream fout(RESULTS_LOG);

    std::string iline;
    std::string pline;
    int e = 0;

    if (fin.is_open() && fpr.is_open()) {
        while (std::getline(fin, iline) && std::getline(fpr, pline)) {
            if (e % CHECKPOINT == 0)
                std::cout << "Processing input " << e << std::endl;
            char *cstr = const_cast<char *>(iline.c_str());
            char *current;
            std::vector<float> in;
            current = strtok(cstr, " ");
            while (current != NULL) {
                in.push_back(atof(current));
                current = strtok(NULL, " ");
            }
            cstr = const_cast<char *>(pline.c_str());
            std::vector<float> pr;
            current = strtok(cstr, " ");
            while (current != NULL) {
                pr.push_back(atof(current));
                current = strtok(NULL, " ");
            }
            // Ensure the size of in is not greater than bo0_map size
            size_t minSize = std::min(in.size(), static_cast<size_t>(input_t::size)); // Access size as a static member

            for (size_t i = 0; i < minSize; ++i) {
                // Perform type conversion and scale appropriately to fit within ap_fixed<16,6>
                (*bo0_map)[i] = static_cast<ap_fixed<16, 6>>(in[i]); // Assuming in[i] fits within range of ap_fixed<16,6>
            }

            // hls-fpga-machine-learning insert top-level-function
//////////////////// Run on HW start ////////////////////
            // Synchronize buffer content with device side
            std::cout << "synchronize input buffer data to device global memory\n";
        
            bo0.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        //    bo1.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        
            std::cout << "Execution of the kernel\n";
            //auto run = krnl(bo0, bo1, bo_out, DATA_SIZE);
            auto run = krnl(bo0, bo_out, DATA_SIZE);
            run.wait();
        
            // Get the output;
            std::cout << "Get the output data from the device" << std::endl;
            bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        
            // Print contents of bo0_map 
            std::cout << "Contents of bo0_map (Input):" << std::endl;
            for (int i = 0; i < DATA_SIZE; ++i) {
                for(size_t j = 0; j < N_INPUT_1_1; j++){
                    std::cout << bo0_map[i][j] << " ";
                }
            }
            std::cout << std::endl;
        
            std::cout << "Contents of bo0_out_map (Output):" << std::endl;
            for (int i = 0; i < DATA_SIZE; ++i) {
                for(size_t j = 0; j < N_LAYER_8; j++){
                    std::cout << bo0_out_map[i][j] << " ";
                }
            }
            std::cout << std::endl;
        
            std::cout << "TEST END\n";
            //////////////////// Run on HW end ////////////////////

            if (e % CHECKPOINT == 0) {
                std::cout << "Predictions" << std::endl;
                // hls-fpga-machine-learning insert predictions
                for(int i = 0; i < N_LAYER_8; i++) {
                  std::cout << pr[i] << " ";
                }
                std::cout << std::endl;
                std::cout << "Quantized predictions" << std::endl;
            }
            e++;

        }

        delete bo0_map; // Don't forget to release memory if dynamically allocated

        fin.close();
        fpr.close();
    } else {
        std::cout << "INFO: Unable to open input/predictions file, using default input." << std::endl;

//////////////////// Run on HW start ////////////////////
    bo0_map = {0};

    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";

    bo0.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo0, bo_out, DATA_SIZE);
    run.wait();

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);


    std::cout << "Contents of bo0_out_map (Output):" << std::endl;
    for (int i = 0; i < DATA_SIZE; ++i) {
        for(size_t j = 0; j < N_LAYER_8; j++){
            std::cout << bo0_out_map[i][j] << " ";
        }
    }
    std::cout << std::endl;

    std::cout << "TEST END\n";
//////////////////// Run on HW end ////////////////////
    }
    fout.close();
    std::cout << "INFO: Saved inference results to file: " << RESULTS_LOG << std::endl;
/////////////////////////// From HLS4ML test end ///////////////////////////
    return 0;
}