.PHONY: clean

all: clean
	python3 tuto_vitis_stream.py

build:
	python3 tuto_vitis_stream.py

app.exe: ./prj_tuto_vitis_stream/PrjTutoVitisStream_host.cpp
#	g++ -Wall -g -std=c++11 ./prj_tuto_vitis_stream/PrjTutoVitisStream_host.cpp ./prj_tuto_vitis_stream/event_timer.cpp ./prj_tuto_vitis_stream/xilinx_ocl_helper.cpp -o app.exe 
#	g++ -Wall -g -std=c++11 ./prj_tuto_vitis_stream/PrjTutoVitisStream_host.cpp ./prj_tuto_vitis_stream/PrjTutoVitisStream_bridge.cpp ./common/includes/xcl2/xcl2.cpp -o app.exe
	g++ -Wall -g -std=c++11 ./prj_tuto_vitis_stream/PrjTutoVitisStream_host.cpp -o app.exe \
		-I ./prj_tuto_vitis_stream/firmware/ap_types -I/opt/xilinx/xrt/include \
		-L${XILINX_XRT}/lib/ -lOpenCL -pthread -lrt -lstdc++

PrjTutoVitisStream_kernel.xo: ./prj_tuto_vitis_stream/PrjTutoVitisStream_kernel.cpp
#	v++ -c -t hw --platform  xilinx_vck5000_gen4x8_qdma_2_202220_1 --config ./prj_tuto_vitis_stream/accelerator_card.cfg -k PrjTutoVitisStream_kernel -I ./prj_tuto_vitis_stream ./prj_tuto_vitis_stream/PrjTutoVitisStream_kernel.cpp -I./prj_tuto_vitis_stream/firmware/ap_types/ -I./prj_tuto_vitis_stream/firmware/nnet_utils -I ./prj_tuto_vitis_stream/firmware/weights  -o PrjTutoVitisStream_kernel.xo
	v++ --target hw --compile -I"./prj_tuto_vitis_stream" --config ./prj_tuto_vitis_stream/accelerator_card.cfg -o"build/PrjTutoVitisStream_kernel.xo" "./prj_tuto_vitis_stream/PrjTutoVitisStream_kernel.cpp"

PrjTutoVitisStream_kernel.xsa: ./build/PrjTutoVitisStream_kernel.xo
	v++ -l -t hw --platform  xilinx_vck5000_gen4x8_qdma_2_202220_1 --config ./prj_tuto_vitis_stream/accelerator_card.cfg ./build/PrjTutoVitisStream_kernel.xo -o PrjTutoVitisStream_kernel.xsa

PrjTutoVitisStream_kernel.xclbin: ./PrjTutoVitisStream_kernel.xsa
	v++ --package -t hw --config ./prj_tuto_vitis_stream/accelerator_card.cfg ./PrjTutoVitisStream_kernel.xsa -o PrjTutoVitisStream_kernel.xclbin

emconfig.json:
	emconfigutil --platform  xilinx_vck5000_gen4x8_qdma_2_202220_1 --nd 1

clean:
	rm -rf prj_tuto_vitis_stream/
	rm -f KERAS_3layer.json KERAS_3layer_weights.h5  prj_tuto_vitis_stream.tar.gz
	rm -rf PrjTutoVitisStream_host* PrjTutoVitisStream_kernel* .exe *json *csv *log *summary _x xilinx* .run .Xil .ipcache *.jou

# Unless specified, use the current directory name as the v++ build target
TARGET ?= $(notdir $(CURDIR))
