gcc -O2 -pthread ex3.c -o ex3 || exit 1
n=10000000
> ex3_res.txt
for m in 1 2 4 10 100; do
    echo "n=$n m=$m" | tee -a ex3_res.txt
    { time ./ex3 $n $m ; } 2>&1 | tee -a ex3_res.txt
    echo | tee -a ex3_res.txt
done
