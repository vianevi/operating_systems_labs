gcc -O2 -pthread ex4.c -o ex4 || exit 1
n=10000000
> ex4_res.txt
for m in 1 2 4 10 100; do
    echo "n=$n m=$m" | tee -a ex4_res.txt
    { time ./ex4 $n $m ; } 2>&1 | tee -a ex4_res.txt
    echo | tee -a ex4_res.txt
done
