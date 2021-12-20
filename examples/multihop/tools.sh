#!/bin/bash

beacons(){
    curl -s --noproxy '*' http://192.168.4.1/peers.json | jq -r ".neighbours[$1].since, .neighbours[$1].beacons" | tr '\n' ' ' && echo
}
nbeacons(){
    while true
    do 
        beacons $1
        sleep 1s
    done
}

summarize(){
    h=(`head -n1 $1`)
    t=(`tail -n1 $1`)
    d=`echo ${t[0]} - ${h[0]} | bc`
    b=`echo ${t[1]} - ${h[1]} | bc` 
    bpm=`echo "scale=2; 60*$b/$d" | bc`
    echo $d seconds, $b beacons $bpm bpm
    echo 
}

summarize_all(){
    for f in results/*.csv
    do 
        echo $f `summarize $f`
    done
}

export -f beacons
export -f nbeacons
export -f summarize
export -f summarize_all