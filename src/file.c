#include <stdio.h>
#include "matlangc_functions.h"
int main(){
// # simple pageranking
 // # algorithm
 Matrix A;
create_matrix(&A,3,3);
Matrix T;
create_matrix(&T,1,1);
Matrix x;
create_matrix(&x,3,1);
Matrix y;
create_matrix(&y,3,1);
float r;
float i;
A.arr[0][0] = 0.5;
A.arr[0][1] = 0;
A.arr[0][2] = 0.5;
A.arr[1][0] = 0;
A.arr[1][1] = 0;
A.arr[1][2] = 0.5;
A.arr[2][0] = 0.5;
A.arr[2][1] = 1;
A.arr[2][2] = 0;
x.arr[0][0] = 1;
x.arr[1][0] = 1;
x.arr[2][0] = 1;
for (i = 1; i <= 10; i+=1) {
	assign_matrix(y,multiply(A,x));
	assign_matrix(T,multiply(transpose(subtraction(y,x)),subtraction(y,x)));
	r = sqrt(get_matrix_elem(T, 1, 1));
	print_integer(r);
	assign_matrix(x,y);
	}
printf("%s\n","----------");
print_matrix(&x);

}