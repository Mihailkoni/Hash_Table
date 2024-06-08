#include <iostream> // база
#include "Hash_Table2.h"
using namespace std;

void Call_Hash_Table() {
    Hash_Table HT(10);
    Node n;
    HT.read_file("Input_Hash_Table.txt");
    HT.write_file("Output_Hash_Table.txt");
    HT.print();
}

int main() {
    Call_Hash_Table();
    return 0;
}