bin/			- 執行檔目錄
src/			- 程式碼目錄
png/			- 預先以gunplot繪製5個case產生的floorplan結果
input_pa2/		- 測資
testall.sh		- 執行腳本
Makefile		- makefile
report.pdf		- report
readme.txt		- 本文件

compile,

	make

execute,

	./bin/fp [alpha] [.block] [.net] [output]
	
	Ex ./bin/fp 0.5 input_pa2/ami33.block input_pa2/ami33.nets result_ami33.rpt


執行後會在目錄底下產生
floorplan.gp
line
floorplan.png
result_ami33.rpt

floorplan.gp, line 為使用 gnuplot 繪圖所需的文件
floorplan.png 為繪製結果，紅線代表block在B* tree的父子關係。