n=${1:-3}
gcc publisher.c -o publisher && gcc subscriber.c -o subscriber || exit 1

if command -v gnome-terminal > /dev/null; then
    for ((i = 1; i <= n; i++)); do
        gnome-terminal -- bash -c "./subscriber $i; exec bash"
    done
    ./publisher $n
else
    ./publisher $n &
    PUB=$!
    sleep 1
    for ((i = 1; i <= n; i++)); do
        ./subscriber $i > sub$i.log &
    done
    wait $PUB
    sleep 1
    for ((i = 1; i <= n; i++)); do
        echo "--- sub$i.log ---"
        cat sub$i.log
    done
fi