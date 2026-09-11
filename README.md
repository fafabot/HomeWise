# HomeWise - prototipo fisico com ESP8266

## Objetivo

A HomeWise monitora o consumo residencial de agua e energia. O controlador oficial do prototipo fisico e o **LOLIN NodeMCU V3 com ESP8266**, desenvolvido no VS Code com PlatformIO.

## O que ja funciona

- Contagem dos pulsos fisicos do YF-S201 por interrupcao.
- Conversao inicial de 450 pulsos para 1 litro, com calibracao futura.
- Leitura do PZEM-004T v3 por `SoftwareSerial`.
- Exibicao dos valores no OLED e no Monitor Serial.
- Historico de ate 31 dias em memoria RAM.
- Media diaria e previsao mensal.
- Indicacao de consumo anormal acima de 20% da media anterior.
- Dia acelerado de 10 segundos para testar a logica durante o desenvolvimento.

## Controlador e PlatformIO

- Placa: LOLIN NodeMCU V3
- Microcontrolador: ESP8266
- Board ID: `nodemcuv2`
- Monitor Serial: 115200 baud

## Ligacoes de baixa tensao

### OLED SSD1306

| OLED | NodeMCU V3 |
| --- | --- |
| VCC | 3V3 |
| GND | GND |
| SDA | D2 / GPIO4 |
| SCL | D1 / GPIO5 |

Endereco I2C configurado: `0x3C`.

### Sensor de agua YF-S201

| YF-S201 | Ligacao |
| --- | --- |
| Vermelho | 5 V regulados |
| Preto | GND |
| Amarelo | D5 / GPIO14 atraves de conversao de 5 V para 3,3 V |

O sinal amarelo nao deve ser ligado diretamente ao ESP8266. Pode ser usado um conversor de nivel ou um divisor com 10 kohms entre o sinal e D5 e 20 kohms entre D5 e GND.

### PZEM-004T v3 - comunicacao

| PZEM | NodeMCU V3 |
| --- | --- |
| 5V | 5 V regulados |
| GND | GND |
| TX | D6 / GPIO12 (RX do ESP8266) |
| RX | D7 / GPIO13 (TX do ESP8266) |

Use conversao de nivel logico entre a UART de 5 V do PZEM e o ESP8266 de 3,3 V.

## Teste fisico

1. Atualize a branch `main` local e abra a pasta que contem `platformio.ini`.
2. Execute `Clean`, `Build` e `Upload` no ambiente `nodemcuv2`.
3. Abra o Monitor Serial em 115200 baud.
4. Teste primeiro o NodeMCU e o OLED.
5. Para simular um pulso sem o YF-S201, toque rapidamente D5 no GND.
6. Conecte e calibre o YF-S201 com um volume conhecido de agua.
7. Deixe o teste do PZEM para o final.

Sem o PZEM respondendo, os valores de tensao, corrente e potencia permanecem em zero.

## Seguranca eletrica

O lado de 127/220 V do PZEM nao deve ser montado em protoboard. A ligacao da rede depende da versao de 10 A ou 100 A do modulo e deve ser realizada com supervisao qualificada, isolamento e protecao adequados.

## Arquitetura planejada

Sensor de vazao / PZEM -> ESP8266 -> Wi-Fi -> API REST -> MySQL -> Dashboard

## Proximas etapas

1. Calibrar o YF-S201.
2. Validar a comunicacao e as medicoes do PZEM-004T.
3. Substituir o dia acelerado por data e hora reais.
4. Implementar Wi-Fi e envio HTTP/JSON.
5. Integrar API, MySQL e dashboard.

