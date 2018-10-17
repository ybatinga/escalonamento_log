/*
 * Saida.cpp
 *
 * Cada objeto Saida representa uma linha do arquivo de saida
 *
 */


#include "Saida.h"
#include<string> // for string class
using namespace std;

Saida::Saida() {
	// TODO Auto-generated constructor stub
}

Saida::Saida(string at, string val) {
    this->at = at;
    this->val = val;
}

void Saida::SetVal(string val) {
    this->val = val;
}

string Saida::GetVal() const {
    return val;
}

void Saida::SetAt(string at) {
    this->at = at;
}

string Saida::GetAt() const {
    return at;
}


