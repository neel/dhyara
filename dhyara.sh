#!/bin/bash

dhyara(){
    curl --noproxy '*' http://192.168.4.1/command -d "$*"
    echo ""
}