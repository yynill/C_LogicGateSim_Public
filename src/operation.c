#include "operation.h"

int nullGate(int a, int b)  { (void)a; (void)b; return 0; }
int noteNode(int a, int b)  { (void)a; (void)b; return 0; }
int lightNode(int a, int b) { (void)a; (void)b; return 0; }
int switchNode(int a, int b){ (void)a; (void)b; return 0; }

int notGate(int a, int b)   { (void)b; return !a; }
int andGate(int a, int b)   { return a && b; }
int nandGate(int a, int b)  { return !(a && b); }
int orGate(int a, int b)    { return a || b; }
int norGate(int a, int b)   { return !(a || b); }
int xorGate(int a, int b)   { return a != b; }
int xnorGate(int a, int b)  { return a == b; }
