#!/bin/bash

for file in samples/*.idl; do
    cp "samples/$(basename $file).cpp" .
done

for file in *.idl; do
    base=$(basename $file .idl)
    make "$base""client"
    make "$base""server"
done

for file in *.idl; do
    echo "Testing $base"
    # if running on `comp117-02` do server else do client 
    if [[ $hostname == "comp117-02" ]]; then
        base=$(basename $file .idl)
        ./"$base""server" | tee "$base""server.out" 
    else
        base=$(basename $file .idl)
        ./"$base""client" "comp117-02" && ./"$base""client" "comp117-02" | tee "$base""client.out"
        
    fi
done