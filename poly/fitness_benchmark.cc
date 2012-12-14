/*
  Simple fitness benchmarks.

  L2 (on x201), compiled with -O3
    SIZE = 100, TIME = 257 ms
    SIZE = 200, TIME = 1006 ms
    SIZE = 400, TIME = 4008 ms
    SIZE = 800, TIME = 16725 ms

  same, compiled with -O3 -funroll-loops:
    SIZE = 100, TIME = 233 ms
    SIZE = 200, TIME = 880 ms
    SIZE = 400, TIME = 3514 ms
    SIZE = 800, TIME = 13489 ms
 */

#include "image/image.h"
#include "poly/fitness_l2_with_focii.h"

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <vector>

image::Image* RandomImage(int size) {
  unsigned char* pixels = new unsigned char[3 * size * size];
  for (int i = 0; i < size * size; ++i) {
    pixels[3 * i + 0] = rand() % 256;
    pixels[3 * i + 1] = rand() % 256;
    pixels[3 * i + 2] = rand() % 256;
  }
  return new image::Image(pixels, size, size);
}

void RunBenchmark(const poly::Fitness* fitness,
                  int size,
                  int num_iter) {
  std::unique_ptr<image::Image> image1(RandomImage(size));
  std::unique_ptr<image::Image> image2(RandomImage(size));

  std::chrono::time_point<std::chrono::system_clock> start, end;

  start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < num_iter; ++i) {
    fitness->Evaluate(image1.get(), image2.get());
  }
  end = std::chrono::high_resolution_clock::now();

  int duration_milli =
      std::chrono::duration_cast<std::chrono::milliseconds>(
          end - start).count();
  std::cout << "SIZE = " << size
            << ", TIME = " << duration_milli << " ms" << std::endl;
}

int main(int argc, char** argv) {
  poly::Fitness* fitness = new poly::FitnessL2WithFocii();
  std::vector<int> sizes({100, 200, 400, 800});
  
  for (unsigned int i = 0; i < sizes.size(); ++i) {
    RunBenchmark(fitness, sizes[i], 1000);
  }
  
  delete fitness;
}
