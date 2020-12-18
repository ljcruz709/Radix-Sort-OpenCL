__kernel void radix(__global int* inData, __global int* outData, __global int* digit){
	outData[get_global_id(0)] = inData[get_global_id(0)] / digit[0];
	outData[get_global_id(0)] = outData[get_global_id(0)] % 10;
}

__kernel void radix2(__global int* inData, __global int* outData, __global int* offset){
	outData[get_global_id(0) + offset[0]] = inData[get_global_id(0)];
}