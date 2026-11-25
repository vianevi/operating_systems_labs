date
sleep 3
mkdir -p ~/week01/root_dir

date
sleep 3
ls -tr / > ~/week01/root_dir/root.txt

date
sleep 3
mkdir -p ~/week01/home_dir

date
sleep 3
ls -tr ~ > ~/week01/home_dir/home.txt

echo "===== content of root.txt ====="
cat ~/week01/root_dir/root.txt

echo "===== content of home.txt ====="
cat ~/week01/home_dir/home.txt

echo "===== items of the new folders ====="
ls -l ~/week01/root_dir ~/week01/home_dir