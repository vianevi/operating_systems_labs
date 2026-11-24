### 1. Searching recursively for file names with "task" inder the path "/proc" and saving the output to the "output.txt" and errors to the "errors.txt":

find \proc -name "*task*" > output.txt 2> errors.txt

### 2. Printing the first 3 lines from the files "output.txt" and "errors.txt" and sorting them in alphabetic order:

cat output.txt errors.txt | sort | head -n 3

**Output:**

/proc/1/task \
/proc/159/task \
/proc/160/task
