gcc pager.c -o pager && gcc mmu.c -o mmu || exit 1

./pager 4 2 > pager_output.txt &
pager_pid=$!
sleep 1
./mmu 4 R0 R1 W1 R0 R2 W2 R0 R3 W2 $pager_pid | tee mmu_output.txt
wait $pager_pid
echo "================ pager output ================"
cat pager_output.txt
