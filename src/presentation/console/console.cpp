#include<iostream>
#include "../../core/tests/test_setup.cpp"
#include "../../core/shared/global.cpp"

#ifndef CONSOLE_APP
#define CONSOLE_APP


void get_alg_option(int* alg_option){
    std::cout << "Seleccione la opcion deseada" << "\n";
    std::cout << "1 Ejecutar algoritmo BIC" << "\n";
    std::cout << "2 Ejecutar algoritmo SAC" << "\n";
    int option; 
    std::cin >> option;
    *alg_option = option; 
}


void get_cipher_option(int* cipher_option){
    std::cout << "Seleccione la opcion deseada" << "\n";
	std::cout << "1 Evaluar cifrador RC4" << "\n";
	std::cout << "2 Evaluar cifrador HC256" << "\n";
    int option; 
    std::cin >> option;
    *cipher_option = option; 
}

void get_n_m_l(int cipher_opt, int alg_option, int* n, int* m, int* l){
    switch (cipher_opt)
    {
    case HC256_CHIPHER_OPTION:
        cout << "Entrar parametro n en intervalo [8,2048] divisible por 8" << endl;
        cin >> *n;
        if(*n<8 || *n>2048 || *n%8!=0){         
            throw runtime_error("Entrada incorrecta");
        }
        cout << "Entrar parametro m divisible por 8" << endl;
        cin >> *m;
        if(*m > MAX_SIZE){
            throw runtime_error("Maximo valor m excedido");
        }
        if(*m%8!=0){
            throw runtime_error("Entrada incorrecta");
        }
        cout << "Entrar parametro L" << endl;
        cin >> *l;
        if(*l > MAX_L){
            cout << "Maximo valor L excedido" <<endl;
            throw runtime_error("Maximo valor L excedido");
        }
    break;
    case RC4_CHIPHER_OPTION:
        *n = 256;
        cout << "Parametro n fijado a 256" << endl;
        cout << "Entrar parametro m en intervalo [32,512] divisible por 32" << endl;
        cin >> *m;
        if(*m<32 || *m>512 || *m%32!=0){
            throw runtime_error("Entrada incorrecta");
        }
        cout << "Entrar parametro L" << endl;
        cin >> *l;
        if(*l > MAX_L){
            throw runtime_error("Maximo valor L excedido");
        }    
        break;
    default:
        throw runtime_error("Entrada incorrecta");
    }
}

int console_run(){
    std::cout << "// CONSOLE \n"; 
    std::cout << "Console application running \n"; 
    int n, m, l, alg_option, cipher_opt;

    get_alg_option(&alg_option); 
    get_cipher_option(&cipher_opt); 
    get_n_m_l(cipher_opt, alg_option, &n, &m, &l); 
    
    int pow = 1;
    for (int exp = 1; exp <= n; exp++) {
        pow*=2;
        if(pow>=l) break;
    }
    if(pow < l) {
        std::cout << "2^n = "<< pow << " < L" << "\n";
        std::cout << "Entrada incorrecta" <<"\n";
        return 0;
    }

    std::cout << "parametros n=" << n << " m=" << m << " L=" << l << "\n";
    TestSetUp config = {n, m, l, alg_option, cipher_opt};
    make_test(&config); 
    return 0;
}

#endif