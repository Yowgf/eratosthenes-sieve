#!/bin/bash

usage() {
    printf "Usage: <script> <num-processes> <llim> <rlim> <step>"
}

run() {
    numProcesses=$1
    testLeftLim=$2
    testRightLim=$3
    testRangeStep=$4

    if [[ $numProcesses = "" ||\
        $testLeftLim = "" || $testRightLim = "" ||\
        $testRangeStep = "" ]]; then
        echo $(usage)
        exit 1
    fi

    execPath="./build/eratosthenes-sieve"
    mpiExec="mpiexec"
    mpiArgs="-n $numProcesses"
    testDir="tests"
    outFileBaseName=$testDir"/"

    execBaseArgs=""
    runXTimes=10
    i=$testLeftLim
    while (( i < $testRightLim )); do

        for j in `seq 1 ${runXTimes}`; do            
            # "t" means we are measuring time!
            execCommand="$mpiExec $mpiArgs $execPath $execBaseArgs
                           $i t"
            
            echo $execCommand
            $execCommand > $outFileBaseName$i-$numProcesses-run$j.out
        done

        i=$(( $i * $testRangeStep ))
    done

}

main() {
    run $@
}

main $@
