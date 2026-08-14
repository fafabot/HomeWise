# HomeWise — Simulação Wokwi

## Objetivo

Simular a lógica inicial da Central HomeWise antes da chegada dos componentes físicos.

### Água
Botão azul → representa os pulsos de um sensor de vazão → ESP32 → cálculo em litros.

### Energia
Botão vermelho → representa ligar/desligar uma carga (liquidificador) → ESP32 → cálculo de corrente, potência e energia.

## Importante

O PZEM-004T e a física do sensor de vazão não são simulados fisicamente neste primeiro protótipo. Os valores representam o que o ESP32 receberia dos sensores reais.

## Controles

### Botão azul — PULSO AGUA
Cada toque representa 10 pulsos.

A simulação utiliza 450 pulsos = 1 litro.

### Botão vermelho — ENERGIA ON/OFF
Liga/desliga uma carga virtual de 500 W em 127 V.

## Próximas etapas

1. Adicionar Wi-Fi virtual.
2. Enviar JSON para uma API.
3. Criar banco de dados.
4. Criar dashboard.
5. Adicionar médias e previsão mensal.
6. Adicionar alertas.
7. Quando os componentes chegarem, substituir a simulação pelos sensores reais.

## Arquitetura

Sensor de vazão → ESP32 → Wi-Fi → API → Banco → Dashboard

PZEM + CT → ESP32 → Wi-Fi → API → Banco → Dashboard
