#!/bin/bash

base=$1
rate=$2
rm *.dat.* classifier.opf

training=.5
testing=$(bc -l <<< "1 - $training")

printf "\n########\n"
printf "Test #$3\n"
printf "########\n"
printf "Your base is $base; train = 0$training; test = 0$testing; rate = $rate\n"

if [ $3 -eq 1 ]; then
    echo "opf" >> testing.dat.acc
    echo "protos+" >> testing.dat.acc
    echo "protos" >> testing.dat.acc
    echo "rand" >> testing.dat.acc
    echo "kmeans" >> testing.dat.acc

    for i in `seq 1 10`; do
        ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1

        echo "--------------------------------------------------------------------------------"
        echo "---------" >> testing.dat.acc
            
        # Test opf model
        ./LibOPF_Kaue/bin/opf_train training.dat
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

        # Test prototypes plus model
        ./LibOPF_Kaue/bin/opf_train_prototypesplus training.dat $rate
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

        # Test protos model
        ./LibOPF_Kaue/bin/opf_train_prototypes training.dat $rate
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

        # Test rand model
        ./LibOPF_Kaue/bin/opf_train_random training.dat
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

        # Test kmeans model
        ./LibOPF_Kaue/bin/opf_train_kmeans training.dat
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat
    done
    mv testing.dat.acc $base.acc

elif [ $3 -eq 2 ]; then
    ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1
            
    # Test opf model
        ./LibOPF_Kaue/bin/opf_train training.dat
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

        # Test prototypes plus model
        ./LibOPF_Kaue/bin/opf_train_prototypesplus training.dat $rate
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

        # Test protos model
        ./LibOPF_Kaue/bin/opf_train_prototypes training.dat $rate
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

        # Test rand model
        ./LibOPF_Kaue/bin/opf_train_random training.dat
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

        # Test kmeans model
        ./LibOPF_Kaue/bin/opf_train_kmeans training.dat
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

elif [ $3 -eq 3 ]; then
    ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1

    # Test protos model
    ./LibOPF_Kaue/bin/opf_train_prototypes training.dat $rate
    ./LibOPF_Kaue/bin/opf_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat

elif [ $3 -eq 4 ]; then
    ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1

    # Test opf model
    ./LibOPF_Kaue/bin/opf_train training.dat
    ./LibOPF_Kaue/bin/opf_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat

elif [ $3 -eq 5 ]; then
    ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1

    # Test rand model
    ./LibOPF_Kaue/bin/opf_train_prototypes training.dat $rate
    ./LibOPF_Kaue/bin/opf_train_random training.dat
    ./LibOPF_Kaue/bin/opf_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat

elif [ $3 -eq 6 ]; then
    ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1

    # Test prototypes plus model
    ./LibOPF_Kaue/bin/opf_train_prototypesplus training.dat $rate
    ./LibOPF_Kaue/bin/opf_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat

elif [ $3 -eq 7 ]; then
    ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1

    # Test prototypes plus model
    ./LibOPF_Kaue/bin/opf_train_prototypes training.dat $rate
    ./LibOPF_Kaue/bin/opf_train_kmeans training.dat
    ./LibOPF_Kaue/bin/opf_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat
fi

# rm *.dat.* classifier.opf