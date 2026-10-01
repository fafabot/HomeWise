#ifndef COMUNICACAO_H
#define COMUNICACAO_H

struct DadosComunicacao {
    float agua;
    float vazao;
    float energia;
    float potencia;
};

void iniciarComunicacao();
void atualizarComunicacao();

bool wifiConectado();

DadosComunicacao obterDadosSimulados();
void enviarDadosSimulados();

#endif
