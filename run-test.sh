#!/bin/bash
base=$1
rm *.dat* classifier.opf

# for i in `seq 1 9`; do
#     training=$(bc -l <<< "$i/10")
#     testing=$(bc -l <<< "1 - $training")

#     # echo "your base is $base and train is $training and test is $testing"
#     ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1
    
#     # Test opf model
#     ./LibOPF_Kaue/bin/opf_train training.dat
#     ./LibOPF_Kaue/bin/opf_classify testing.dat
#     ./LibOPF_Kaue/bin/opf_accuracy testing.dat

#     # Test opf-prototypes model
#     ./LibOPF_Kaue/bin/opf_train prototypes.dat
#     ./LibOPF_Kaue/bin/opf_classify testing.dat
#     ./LibOPF_Kaue/bin/opf_accuracy testing.dat

#     # Test random model
#     # Test k-means model

#     echo "\n" >> testing.dat.acc
# done 
    
# mv testing.dat.acc $base.acc
# rm *.dat* classifier.opf

training=.5
testing=$(bc -l <<< "1 - $training")

echo "your base is $base and train is $training and test is $testing"

for i in `seq 1 10`; do
    ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1
        
    # Test opf model
    ./LibOPF_Kaue/bin/opf_train training.dat
    ./LibOPF_Kaue/bin/opf_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat

    # Test knn model
    ./LibOPF_Kaue/bin/opf_train training.dat
    ./LibOPF_Kaue/bin/opf_knn_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat

    echo "---------" >> testing.dat.acc
done