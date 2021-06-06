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
    export CILK_NWORKERS=$THREADS && ./cilk/cilk_max $N
    # RUN HPX
    ./hpx/hpx_max $N --hpx:threads=$THREADS
    echo "---"
done