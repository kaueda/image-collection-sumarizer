all: Multiplos_classificadores
	gcc main_ivote.c -o saida OPF.o common.o realheap.o sgctree.o gqueue.o set.o subgraph.o Ivote.o -lm

Multiplos_classificadores:
	gcc Ivote.c -c
run:
	./saida <in

clean:
	rm Train*.opf -f
	rm classify*.txt -f
	rm saida -f
	rm labels_out*.txt -f
	rm testing*.dat -f
	rm training*.dat -f
	rm evaluating*.dat -f
	
