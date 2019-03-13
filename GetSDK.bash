#! /bin/sh

bin_path="../../../hy_bin/latest/"

for lib_name in "hyc"
do
	rm -rf "./ext/"$lib_name
	mkdir -p "./ext/"$lib_name
    mkdir -p "./bin"
    cp -rf $bin_path$lib_name"/include"     "./ext/"$lib_name
	cp -rf $bin_path$lib_name"/ios/"*       "./bin/"
done

echo "is done"

 
