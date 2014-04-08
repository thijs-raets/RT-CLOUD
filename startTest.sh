#!/bin/bash
#1 start test
cd /test
./setup.out > testCodeOutput.sh
chmod +x testCodeOutput.sh
./createTest.sh &
while [ ! -e 1/1.sh ]; do sleep 1; done
while [ ! -e 1/2.sh ]; do sleep 1; done
cp 1/1.sh 1.sh
cp 1/2.sh 2.sh
chmod +x 1.sh
chmod +x 2.sh

#2 mount test on VM
#ssh root@\VM3 'sshfs root@Domain-0:/test /test -o nonempty;'
#ssh root@\VM2 'sshfs -p 55000 root@Domain-0:/test /test -o nonempty;'

#3 start test on VM
echo "starting tests on VM3"
ssh root@\VM3 'cd /test; ./1.sh &' &
echo "starting tests on VM6"
ssh root@\VM6 'cd /test; ./2.sh &' &

