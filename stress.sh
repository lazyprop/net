for f in $(seq 1 $1);
do
    ./streamclient > /dev/null &
done
