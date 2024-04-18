#! /bin/bash

# Define the associative array
declare -A str_map
str_map[0]="ami33"
str_map[1]="ami49"
str_map[2]="apte"
str_map[3]="hp"
str_map[4]="xerox"

mkdir -p png

for i in {0..4}
do
    # Define a string based on the value of i
    str=${str_map[$i]}

    # Use the string in your command, if needed
    echo "Processing with $str"  # Example usage of the string

	./bin/fp 0.5 input_pa2/$str.block input_pa2/$str.nets result_$str.rpt
    	./evaluator/evaluator.sh input_pa2/$str.block input_pa2/$str.nets result_$str.rpt 0.5
	mv floorplan.png png/$str.png
	rm floorplan.gp
	rm line
done
