/*
 * Saida.h
 *
 * Cada objeto Saida representa uma linha do arquivo de saida
 *
 */

#ifndef SAIDA_H_
#define SAIDA_H_
#include<string> // for string class
using namespace std;

class Saida {
public:
	Saida();
	Saida(string at, string val);
        void SetVal(string val);
        string GetVal() const;
        void SetAt(string at);
        string GetAt() const;

private:
	string at; // atributo. Ex.: X / Y
	string val; // valor do atributo 
};

#endif /* SAIDA_H_ */
