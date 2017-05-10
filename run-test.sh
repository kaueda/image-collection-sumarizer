#!/bin/bash
base=$1
rm *.dat.* classifier.opf

training=.5
testing=$(bc -l <<< "1 - $training")

printf "\nyour base is $base; train = $training and test = $testing\n"
printf "Test #$2\n\n"

if [ $2 -eq 1 ]; then
    for i in `seq 1 10`; do
        ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1
            
        # Test opf model
        ./LibOPF_Kaue/bin/opf_train training.dat
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

        # Test protos model
        ./LibOPF_Kaue/bin/opf_train_prototypes training.dat
        ./LibOPF_Kaue/bin/opf_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

        echo "---------" >> testing.dat.acc
    done
    mv testing.dat.acc $base.acc

elif [ $2 -eq 2 ]; then
    ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1
            
    # Test opf model
    ./LibOPF_Kaue/bin/opf_train training.dat
    ./LibOPF_Kaue/bin/opf_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat

    # Test protos model
    ./LibOPF_Kaue/bin/opf_train_prototypes training.dat
    ./LibOPF_Kaue/bin/opf_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat
fi

# rm *.dat.* classifier.opf