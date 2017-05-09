#!/bin/bash
base=$1
<<<<<<< HEAD
rm *.dat.* classifier.opf
=======
rm *.dat* classifier.opf
>>>>>>> 481d42f8c33780fe930ed7d3b440df6efb2547a1

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
<<<<<<< HEAD
# rm *.dat.* classifier.opf
=======
# rm *.dat* classifier.opf
>>>>>>> 481d42f8c33780fe930ed7d3b440df6efb2547a1

training=.5
testing=$(bc -l <<< "1 - $training")

<<<<<<< HEAD
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
        ./LibOPF_Kaue/bin/opf_knn_classify testing.dat
        ./LibOPF_Kaue/bin/opf_accuracy testing.dat

        echo "---------" >> testing.dat.acc
    done
elif [ $2 -eq 2 ]; then
    ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1
            
=======
echo "your base is $base and train is $training and test is $testing"

for i in `seq 1 10`; do
    ./LibOPF_Kaue/bin/opf_split ./LibOPF_Kaue/bases/$base $training 0 $testing 1
        
>>>>>>> 481d42f8c33780fe930ed7d3b440df6efb2547a1
    # Test opf model
    ./LibOPF_Kaue/bin/opf_train training.dat
    ./LibOPF_Kaue/bin/opf_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat

<<<<<<< HEAD
    # Test protos model
    ./LibOPF_Kaue/bin/opf_train_prototypes training.dat
    ./LibOPF_Kaue/bin/opf_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat
fi
=======
    # Test knn model
    ./LibOPF_Kaue/bin/opf_train training.dat
    ./LibOPF_Kaue/bin/opf_knn_classify testing.dat
    ./LibOPF_Kaue/bin/opf_accuracy testing.dat

    echo "---------" >> testing.dat.acc
done
>>>>>>> 481d42f8c33780fe930ed7d3b440df6efb2547a1
