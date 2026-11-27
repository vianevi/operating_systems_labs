pid=$(cat /tmp/ex1.pid 2>/dev/null)
if [ -z "$pid" ] || ! kill -0 "$pid" 2>/dev/null; then
    echo "ex1 is not running (no valid PID in /tmp/ex1.pid)"; exit 1
fi
echo "ex1 PID = $pid"

found=0
while read -r range perms rest; do
    [[ $perms == rw* ]] || continue
    [[ $rest == *"[stack]"* || $rest == *"[vvar"* ]] && continue
    start=$((16#${range%-*}))
    end=$((16#${range#*-}))

    matches=$(dd if=/proc/$pid/mem bs=4096 skip=$((start / 4096)) count=$(((end - start) / 4096)) 2>/dev/null | grep -a -b -o 'pass:[[:graph:]]\{8\}')
    while read -r line; do
        [ -z "$line" ] && continue
        offset=${line%%:*}
        text=${line#*:}
        printf 'Password: %s   address: 0x%x   (region %s %s)\n' "${text#pass:}" $((start + offset)) "$range" "$perms"
        found=1
    done <<< "$matches"
done < /proc/$pid/maps

if [ $found -eq 1 ]; then
    kill -SIGKILL "$pid" && echo "Sent SIGKILL to $pid"
else
    echo "Password not found"
fi
