#ifndef OPERATION_H
#define OPERATION_H
#pragma once

typedef int (Operation)(int, int);

extern int nullGate(int a, int b);
extern int noteNode(int a, int b);
extern int lightNode(int a, int b);
extern int switchNode(int a, int b);

extern int notGate(int a, int b);
extern int andGate(int a, int b);
extern int nandGate(int a, int b);
extern int orGate(int a, int b);
extern int norGate(int a, int b);
extern int xorGate(int a, int b);
extern int xnorGate(int a, int b);

#endif // OPERATION_H
