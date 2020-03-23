# comsyn
## get started
 - compile "gen_cnf.cpp" by your c++ compiler.
 - install lingeling at https://github.com/arminbiere/lingeling and make sure that a command "plingeling" runs plingeling.
 - describe your problem in a text file. refer to "setting.txt" for example.
 - run the executable with an argument specifying the problem file. "./a.out setting.txt" for example.
 - a result is written in "\_(the name of the problem file).assign" and "\_(the name of the problem file).com".
 the former shows "cycle node data" for each line, and the latter shows "cycle sender_node recipient_node data" for each line.

## problem file
in the problem file, you must specify the following parameters:
the number of cycle, the nubmer of nodes, the adjacent matrix of nodes, the inital assignemnt, and the final assignment.

the example "setting.txt" is shown below. 4 nodes are connected in oneway ring. each path can communicate 1 data in 1 cycle.
each node initially has 1 data and finally has all data i.e. all gather (cf. MPI_Allgather) is performed.
if a node has nothing, insert an empty line.
Our program will generate a CNF file to check whether such communication can be done in the given number of cycles or not.
This problem is satisfiable where each node passes the received data to the next node every cycle.
```
cycle
4

node
4

spx
0 1 0 0
0 0 1 0
0 0 0 1
1 0 0 0

initial
X1
X2
X3
X4

final
X1 X2 X3 X4
X1 X2 X3 X4
X1 X2 X3 X4
X1 X2 X3 X4
```

## problem generator for our convolution problem
we put a python file in a directory "cnn" to generate a problem file for our convolution problem.

a command "python gen_cnn_setting.py matrix_row matrix_column window_row window_column cycle pad_row pad_col stride (option)" generates a problem file "setting.txt"
where the arguments except option are integers: matrix_row and matrix_column specify the size of image, window_row and window_column specify the size of kernel,
cycle is a number of cycles, pad_row and pad_col are numbers of rows and columns for padding nodes (nodes just passing data) at top and left side,
and stride is a size of stride.

for our problem, options "onehot_spx_in" and "systolic" should be used. the former restricts the number of data each node receives in a cycle to at most 1,
 and the latter restricts the number of data each processor keeps to at most 1 while the final condition will be met if each node has received all required data at least once.
 
an option "sparse" is prepared for problems with sparse kernel. all elements in kernel is numbered as the elemnt at row *i* and column *j* to be *j* + *i*\*matrix_col.
note *i* and *j* start from 0. after "sparse", the indices of zero elements are specified splitted by "\_".
for example, "sparse_1_4" means the elements numbered as 1 and 4 are 0 as follows:
```
* 0 *
* 0 *
* * *
```

we also have a visualizer as a python file "gen_png.py".
a command "python gen_png.py (matrix_row+pad_row) (matrix_column+pad_column) cycle _setting.txt.assign _setting.txt.com" generates png files "img\*.png" with \* the cycle count.
please calculate addition in the argument and change the names of result files.

## example
### simple algorithm
 - run a command "python gen_cnn_setting.py 4 4 2 2 4 1 1 1 onehot_spx_in systolic" in a directory "cnn" and make sure "setting.txt" is generated.
 - run your executable with the generated problem file being its argument and make sure it is satisfiable.
 - run a command "python gen_png.py 5 5 4 _setting.txt.assign _setting.txt.com". the png files as below should be generated (some details may differ due to SAT solver).
<img src="https://user-images.githubusercontent.com/18373300/77285682-83949300-6cc9-11ea-8080-e28d074ed6e7.png" width="200">
<img src="https://user-images.githubusercontent.com/18373300/77285715-93ac7280-6cc9-11ea-949e-1c8d83b9edc4.png" width="200">
<img src="https://user-images.githubusercontent.com/18373300/77285732-9b6c1700-6cc9-11ea-8402-8952106b417d.png" width="200">
<img src="https://user-images.githubusercontent.com/18373300/77285734-9d35da80-6cc9-11ea-9a5b-56fe2ca2248c.png" width="200">

### stride
 - run a command "python gen_cnn_setting.py 4 4 2 2 4 1 1 2 onehot_spx_in systolic" instead.
 - the places (processors) where the output pixels are generated are shifted to the center to fill the gaps.
 <img src="https://user-images.githubusercontent.com/18373300/77286220-b12e0c00-6cca-11ea-8230-2a941a197fc7.png" width="200">
 <img src="https://user-images.githubusercontent.com/18373300/77286223-b25f3900-6cca-11ea-9ac1-7c69e7a99463.png" width="200">
 <img src="https://user-images.githubusercontent.com/18373300/77286224-b25f3900-6cca-11ea-851f-7ff27b83518b.png" width="200">
 <img src="https://user-images.githubusercontent.com/18373300/77286226-b25f3900-6cca-11ea-9bf2-fc01714b0e75.png" width="200">

### sparse kernel
 - run a command "python gen_cnn_setting.py 5 5 3 3 7 2 2 1 onehot_spx_in systolic sparse_1_4" instead.
 - the algorithm for 3 \* 3 kernel originally takes 9 cycles, but presence of 2 zero elements reduced the number of cycles to 7.
<img src="https://user-images.githubusercontent.com/18373300/77286650-9dcf7080-6ccb-11ea-8080-be4b38a527e6.png" width="200">
<img src="https://user-images.githubusercontent.com/18373300/77286653-9e680700-6ccb-11ea-86a7-f0b8411be3c1.png" width="200">
<img src="https://user-images.githubusercontent.com/18373300/77286654-9e680700-6ccb-11ea-9262-5968ba882870.png" width="200">
<img src="https://user-images.githubusercontent.com/18373300/77286655-9f009d80-6ccb-11ea-90d2-703e4f79f4d8.png" width="200">
<img src="https://user-images.githubusercontent.com/18373300/77286656-9f009d80-6ccb-11ea-81c4-e7853df000be.png" width="200">
<img src="https://user-images.githubusercontent.com/18373300/77286658-9f993400-6ccb-11ea-860f-8fb56225bd10.png" width="200">
<img src="https://user-images.githubusercontent.com/18373300/77286659-9f993400-6ccb-11ea-9bbe-80c2a0c0d0a2.png" width="200">

## afterword
feel free to ask questions.
the source codes are not difficult at all. please read and modify it according to your purpose.

Yukio Miyasaka (miyasaka at cad.t.u-tokyo.ac.jp)
