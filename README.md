# HomeWise - Simulacao Wokwi

## Objetivo

Simular a logica inicial da Central HomeWise antes da integracao com os componentes fisicos. O prototipo mede consumo de agua e energia, exibe os valores no OLED, armazena um historico diario em memoria e produz analises basicas.

## O que ja funciona

- Contagem simulada de pulsos de agua.
- Conversao de pulsos para litros.
- Carga eletrica virtual de 500 W em 127 V.
- Calculo de tensao, corrente, potencia e energia acumulada.
- Exibicao no OLED e no Monitor Serial.
- Virada de dia simulada a cada 10 segundos.
- Historico de ate 30 dias em memoria RAM.
- Media diaria e previsao mensal.
- Indicacao de consumo anormal quando o dia supera em 20% a media dos dias anteriores.

## Controles

### Botao azul - agua

Cada toque representa 10 pulsos. A simulacao utiliza 450 pulsos por litro.

### Botao vermelho - energia

Liga ou desliga a carga virtual de 500 W.

## Importante

O PZEM-004T e o sensor de vazao ainda nao sao simulados fisicamente. O historico atual fica somente na RAM e e perdido quando o ESP32 reinicia. No primeiro dia, o sistema informa que ainda nao existe historico suficiente para classificar o consumo.

## Arquitetura planejada

Sensor de vazao / PZEM -> ESP32 -> Wi-Fi -> API REST -> MySQL -> Dashboard

## Proximas etapas

1. Adicionar Wi-Fi virtual ao ESP32.
2. Definir e gerar o JSON das leituras.
3. Enviar as leituras para uma API REST.
4. Armazenar os dados no MySQL.
5. Criar o dashboard com graficos e alertas.
6. Integrar e calibrar os sensores fisicos.
