# Strong Scaling

echo "~STRONG SCALING REPORT~"
echo 
# Give the number of elements for strong scaling
# as the first argument. ex. "./max_benchmark.sh 999"
if [ "$1" == "" ]
then
    N=$((10 ** 8))
else
    N="$1"
fi

for i in {0..4}
do
    THREADS=$((2 ** $i))
    # RUN CILK
    export CILK_NWORKERS=$THREADS && ./cilk/cilk_sum $N
    # RUN HPX
    ./hpx/hpx_sum $N --hpx:threads=$THREADS
    echo "---"
done


# Weak Scaling

echo "~WEAK SCALING REPORT~"
echo 
# Give the initial number of elements for weak scaling
# as the first argument. The elements will be doubled
# in each iteration.
if [ "$2" == "" ]
then
    N=$((10 ** 6))
else
    N="$2"
fi

for i in {0..4}
do
    THREADS=$((2 ** $i))
    # RUN CILK
    export CILK_NWORKERS=$THREADS && ./cilk/cilk_sum $N
    # RUN HPX
    ./hpx/hpx_sum $N --hpx:threads=$THREADS
    N=$(($N*2))
    echo "---"
done