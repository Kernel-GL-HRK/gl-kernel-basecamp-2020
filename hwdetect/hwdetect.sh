lsusb > prev.txt
while :
do
        lsusb > next.txt
        diff prev.txt next.txt
        mv next.txt prev.txt
done
