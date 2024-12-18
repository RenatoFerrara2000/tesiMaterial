 
/*
 * Copyright 2020 The TensorFlow Authors. All Rights Reserved.
 * gettimeofday
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "main_functions.h"
#include <chrono>  // Libreria per misurare il tempo
#include <sys/time.h> 
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include "constants.h"
#include "model.hpp"
#include "output_handler.hpp"
#include <tensorflow/lite/micro/micro_log.h>
#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/system_setup.h>
#include <tensorflow/lite/schema/schema_generated.h>
#include <zephyr/kernel.h>
#include <zephyr/timing/timing.h>


/* Globals, used for compatibility with Arduino-style sketches. */
namespace {
	const tflite::Model *model = nullptr;
	tflite::MicroInterpreter *interpreter = nullptr;
	TfLiteTensor *input = nullptr;
	TfLiteTensor *output = nullptr;
	int inference_count = 0;
	long total_inference_time = 0; // To accumulate total inference time

	constexpr int kTensorArenaSize = 2000;
	uint8_t tensor_arena[kTensorArenaSize];
}  /* namespace */

/* The name of this function is important for Arduino compatibility. */
void setup(void)
{
	/* Map the model into a usable data structure. This doesn't involve any
	 * copying or parsing, it's a very lightweight operation.
	 */
	model = tflite::GetModel(g_model);
	if (model->version() != TFLITE_SCHEMA_VERSION) {
		MicroPrintf("Model provided is schema version %d not equal "
					"to supported version %d.",
					model->version(), TFLITE_SCHEMA_VERSION);
		return;
	}

	/* This pulls in the operation implementations we need.
	 * NOLINTNEXTLINE(runtime-global-variables)
	 */
	
	static tflite::MicroMutableOpResolver <1> resolver;
	resolver.AddFullyConnected();

	/* Build an interpreter to run the model with. */
	static tflite::MicroInterpreter static_interpreter(
		model, resolver, tensor_arena, kTensorArenaSize);
	interpreter = &static_interpreter;

	/* Allocate memory from the tensor_arena for the model's tensors. */
	TfLiteStatus allocate_status = interpreter->AllocateTensors();
	if (allocate_status != kTfLiteOk) {
		MicroPrintf("AllocateTensors() failed");
		return;
	}

	/* Obtain pointers to the model's input and output tensors. */
	input = interpreter->input(0);
	output = interpreter->output(0);

	/* Keep track of how many inferences we have performed. */
	inference_count = 0;
		total_inference_time = 0; // Initialize total inference time

}

/* The name of this function is important for Arduino compatibility. */
void loop(void)
{
	    using namespace std::chrono;
timing_t start_time, end_time;
    uint64_t total_cycles;
    uint64_t total_ns;

    timing_init();
    timing_start();
 
 	/* Calculate an x value to feed into the model. We compare the current
	 * inference_count to the number of inferences per cycle to determine
	 * our position within the range of possible x values the model was
	 * trained on, and use this to calculate a value.
	 */
	float position = static_cast < float > (inference_count) /
			 static_cast < float > (kInferencesPerCycle);
	float x = position * kXrange;

	/* Quantize the input from floating-point to integer */
	int8_t x_quantized = x / input->params.scale + input->params.zero_point;
	/* Place the quantized input in the model's input tensor */
	input->data.int8[0] = x_quantized;
    //auto start = high_resolution_clock::now();
    start_time = timing_counter_get();
 
	/* Run inference, and report any error */
	TfLiteStatus invoke_status = interpreter->Invoke();
	if (invoke_status != kTfLiteOk) {
		MicroPrintf("Invoke failed on x: %f\n", static_cast < double > (x));
		return;
	}

    end_time = timing_counter_get();

	
  total_cycles = timing_cycles_get(&start_time, &end_time);
    total_ns = timing_cycles_to_ns(total_cycles);    
    // Accumulate total inference time
    total_inference_time += total_ns / 1'000;;
// Debugging output
MicroPrintf("Start cycles: %llu, End cycles: %llu, Total cycles: %llu\n", start_time, end_time, total_cycles);
MicroPrintf("Total nanoseconds: %llu\n", total_ns);
MicroPrintf("Tempo di inferenza: %llu microsecondi\n", total_ns / 1'000);

    
    // Stampa il tempo di inferenza
 	/* Obtain the quantized output from model's output tensor */
	int8_t y_quantized = output->data.int8[0];
	/* Dequantize the output from integer to floating-point */
	float y = (y_quantized - output->params.zero_point) * output->params.scale;

	/* Output the results. A custom HandleOutput function can be implemented
	 * for each supported hardware target.
	 */
	HandleOutput(x, y);

	/* Increment the inference_counter, and reset it if we have reached
	 * the total number per cycle
	 */
	inference_count += 1;
	if (inference_count >= kInferencesPerCycle) {
		long average_inference_time = total_inference_time / kInferencesPerCycle;
		MicroPrintf("Tempo medio di inferenza: %ld microsecondi.\n", average_inference_time);
		inference_count = 0; // Reset the count
		total_inference_time = 0; // Reset the total time
	}
}

int gettimeofday(struct timeval *tv, void *tz) {
    // Mock implementation, return 0 for success
    if (tv) {
        tv->tv_sec = 0;
        tv->tv_usec = 0;
    }
    return 0;
}
