#!/bin/bash

dhyara(){
    curl -s --noproxy '*' http://192.168.4.1/command -d "$*"
    echo ""
}
export -f dhyara

