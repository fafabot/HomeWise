# HomeWise — Protótipo físico com ESP8266

## Objetivo

A HomeWise é um sistema de monitoramento residencial de consumo de água e energia. O protótipo oficial passa a utilizar o **WEMOS D1 R1 com ESP8266**, aproveitando o hardware físico disponível para os testes e para a apresentação do TCC.

## Controlador

- WEMOS D1 R1
- Microcontrolador ESP8266
- Wi-Fi integrado
- Desenvolvimento com VS Code + PlatformIO

## Água

O sensor de vazão envia pulsos ao ESP8266. O módulo `agua.cpp` conta esses pulsos por interrupção e converte o total em litros.

Configuração inicial:

- Pino: D5 / GPIO14
- Referência inicial: 450 pulsos por litro
- Cada pulso físico conta como 1 evento

O valor de pulsos por litro deverá ser calibrado com testes reais de volume.

## Energia

O PZEM-004T v3 realiza as leituras elétricas e se comunica com o ESP8266 utilizando `SoftwareSerial`.

Configuração:

- RX: D6 / GPIO12
- TX: D7 / GPIO13
- Leituras: tensão, corrente, potência e energia

O sistema utiliza a leitura acumulada do PZEM como referência para calcular o consumo diário sem apagar o contador interno do módulo.

## OLED

O display OLED SSD1306 utiliza comunicação I2C:

- SDA: D2 / GPIO4
- SCL: D1 / GPIO5
- Endereço: 0x3C

## Histórico

A lógica atual mantém o histórico de água e energia em memória. Durante o desenvolvimento, o encerramento de um dia continua acelerado para facilitar os testes da lógica de histórico, médias e futuras análises.

## Arquitetura planejada

Sensor de vazão → ESP8266 → Wi-Fi → API → MySQL → Site HomeWise

PZEM-004T → ESP8266 → Wi-Fi → API → MySQL → Site HomeWise

## Wokwi

A simulação anterior com ESP32 foi preservada apenas como material de referência em `legacy/wokwi-esp32/`.

O protótipo oficial agora é voltado ao ESP8266 físico. A documentação oficial atual do Wokwi não lista o ESP8266 entre os microcontroladores suportados, por isso os arquivos de simulação ESP32 não ficam mais na raiz do projeto.

## Próximas etapas

1. Compilar o projeto para o WEMOS D1 R1.
2. Testar o OLED no ESP8266 físico.
3. Testar e calibrar o sensor de vazão.
4. Validar a comunicação com o PZEM-004T.
5. Substituir o tempo acelerado por controle de data/hora real.
6. Implementar Wi-Fi e envio HTTP/JSON para a API.
7. Integrar banco MySQL e dashboard.
8. Implementar médias, previsão mensal e alertas.

## Segurança elétrica

A parte de medição de energia envolve tensão de rede. A montagem física deve respeitar isolamento, proteção e procedimentos adequados. O ESP8266 trabalha com níveis lógicos de 3,3 V; antes de conectar módulos externos, os níveis elétricos dos sinais devem ser verificados.
