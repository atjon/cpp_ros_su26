//header guard:
//note in headers.cpp that we include fake.h and headers.h,
//which essentially copies the same code twice
//header guards are in place to make sure that if the function is alr defined, 
//to prevent copying the same code again and cause repeat classes or methods
//that aren't allowed to be repeated
#ifndef HEADERS_H
#define HEADERS_H

//Content of .h file, where declaration goes
int add(int x, int y); //method header definition

//General best practices
/*
Don't define variables in header files
Each header should have an intentional purpose
Headers should have any #includes it needs to compile when included in a .cpp file
Document high level descriptions of the corresponding .cpp file
*/

#endif