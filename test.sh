#! /bin/bash
cur=0
function hh(){
    for i in `seq 1 10`
    do
        cur=$((cur+1))
        echo $cur
        sleep 1
    done
}
hh &
sleep 0.1
hh &


