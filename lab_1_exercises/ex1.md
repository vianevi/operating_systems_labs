### 1. Create a directory `week01` in home directory:

```
mkdir -p ~/week01
```

### 2. List last 5 entries in /usr/bin that contain `gcc` on reverse alphabetical order and save results in `~/week01/ex1.txt`

```
ls /usr/bin | grep gcc | sort -r | tail -n 5 > ~/week01/ex1.txt
```

### 3. Concatenate ~/week01/ex1.txt to stdout:

~~~
cat ~/week01/ex1.txt
~~~

## All the commands in terminal:

![](example3.png)
