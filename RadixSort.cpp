#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#include <CL/cl.hpp>
#include <fstream>
#include <iostream>
#include <chrono>


int main() {
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	auto plat = platforms.front();
	std::vector<cl::Device> devices;
	plat.getDevices(CL_DEVICE_TYPE_GPU, &devices);

	auto device = devices.front();


	std::ifstream cin("data.txt");
	std::ifstream kernelFile("RadixSort.cl");
	std::string src(std::istreambuf_iterator<char>(kernelFile), (std::istreambuf_iterator<char>()));

	cl::Program::Sources sources(1, std::make_pair(src.c_str(), src.length() + 1));
	cl::Context context(device);
	cl::Program program(context, sources);
	auto err = program.build("-cl-std=CL1.2");

	std::vector<std::vector<int> > tmp;
	std::vector<int> data;
	std::vector<int> digits;
	std::vector<int> currentDigit(1);
	std::vector<int> offst(1);

	int x;
	
	while (cin >> x) {
		data.push_back(x);
	}
	
	digits.resize(data.size());

	cl::Buffer inBuf;
	cl::Buffer inD; 
	cl::Buffer outBuf(context, CL_MEM_WRITE_ONLY | CL_MEM_HOST_READ_ONLY, sizeof(int) * digits.size());
	cl::Kernel kernel;
	cl::CommandQueue queue(context, device);

	currentDigit.at(0) = 1;


	auto start = std::chrono::high_resolution_clock::now();


	for(int i = 0; i < 4; i++){

		//preparando el kernel para sacar los dígitos
		inBuf = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * data.size(), data.data());
		inD = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * currentDigit.size(), currentDigit.data());
		kernel = cl::Kernel(program, "radix");
		kernel.setArg(0, inBuf);
		kernel.setArg(1, outBuf);
		kernel.setArg(2, inD);


		//ejecutar el kernel
		err = queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(data.size()));
		err = queue.enqueueReadBuffer(outBuf, CL_FALSE, 0, sizeof(int) * digits.size(), digits.data());
		queue.finish();


		currentDigit.at(0) = currentDigit.at(0) * 10;

		//agrupar según el dígito
		tmp.clear();
		tmp.resize(10);

		for (int j = 0; j < data.size(); j++) {
			tmp.at(digits.at(j)).push_back(data.at(j));
		}



		//Pasar de nuevo al vector original
		offst.at(0) = 0;

		for (int j = 0; j < 10; j++) {
			if (tmp.at(j).size() > 0) {
				inBuf = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * tmp.at(j).size(), tmp.at(j).data());
				inD = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_HOST_NO_ACCESS | CL_MEM_COPY_HOST_PTR, sizeof(int) * offst.size(), offst.data());
				kernel = cl::Kernel(program, "radix2");
				kernel.setArg(0, inBuf);
				kernel.setArg(1, outBuf);
				kernel.setArg(2, inD);

				queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(tmp.at(j).size()));
				queue.enqueueReadBuffer(outBuf, CL_FALSE, 0, sizeof(int) * data.size(), data.data());
				queue.finish();

				offst.at(0) += tmp.at(j).size();

			}
		}

	}

	auto end = std::chrono::high_resolution_clock::now();

/*
	for (int i = 0; i < data.size(); i++) {
		std::cout << data.at(i) << "\n";
	}
*/
	
	std::cout << "Microsegundos: " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << '\n';
	std::cout << "Milisegundos: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';
	std::cout << "segundos: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << '\n';

	std::cin.get();


	return 0;
}