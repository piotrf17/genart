# The overall benchmark runs 100 generations with a given random seed,
# averaging 10 runs for various sizes.  The same image is used for all
# sizes and simply upscaled since it's quality has little effect.
# Meant to be run from the root genart directory.

SRC_IMAGE=benchmark/bench_image_400.jpg
TMP_IMAGE=/tmp/genart_benchmark.jpg
NUM_ITERATIONS=3
SIZES="100 200 400 800"

for size in $SIZES
do
  convert $SRC_IMAGE -resize $size $TMP_IMAGE

  total_time=0
  for (( i=1; i <= $NUM_ITERATIONS; i++ ))
  do
    run_time=$(build/genpic --input_image=$TMP_IMAGE --effect_config=benchmark/config --display_step=0 --benchmark 2> /dev/null | grep "BENCHMARK" | grep -o "[0-9]*")
    total_time=$(expr $total_time + $run_time)
  done
  average_time=$(expr $total_time / $NUM_ITERATIONS)

  echo "SIZE = $size, TIME = $average_time ms"
done

# Initial benchmark (x201, 2012/12/13):
#   SIZE = 100, TIME = 400 ms
#   SIZE = 200, TIME = 1450 ms
#   SIZE = 400, TIME = 5249 ms
#   SIZE = 800, TIME = 20744 ms
