# HomeWise - prototipo fisico com ESP8266

## Objetivo

A HomeWise monitora o consumo residencial de agua e energia. O controlador do prototipo fisico e o **LOLIN NodeMCU V3 com ESP8266**, desenvolvido no VS Code com PlatformIO.

## Estado atual

- Contagem de pulsos do YF-S201 por interrupcao e calculo inicial de vazao.
- Leitura do PZEM-004T v3 por SoftwareSerial.
- Exibicao das leituras no OLED SSD1306 e no Monitor Serial.
- Historico e analise locais em memoria RAM, com dia acelerado para testes.
- Firmware preparado para enviar leituras reais via Wi-Fi/HTTP.
- API Node.js/Express com persistencia MySQL e rotas de consulta.
- Dashboard web responsivo; **a interface ainda usa dados demonstrativos** e sera ligada a API em uma etapa posterior.

## Controlador e PlatformIO

- Placa: LOLIN NodeMCU V3
- Microcontrolador: ESP8266
- Board ID: nodemcuv2
- Monitor Serial: 115200 baud

## Ligacoes de baixa tensao

### OLED SSD1306

| OLED | NodeMCU V3 |
| --- | --- |
| VCC | 3V3 |
| GND | GND |
| SDA | D2 / GPIO4 |
| SCL | D1 / GPIO5 |

Endereco I2C configurado: 0x3C.

### Sensor de agua YF-S201

| YF-S201 | Ligacao |
| --- | --- |
| Vermelho | 5 V regulados |
| Preto | GND |
| Amarelo | D5 / GPIO14 atraves de conversao de 5 V para 3,3 V |

**Nao ligue o sinal amarelo diretamente ao ESP8266.** Use conversor de nivel logico ou divisor resistivo dimensionado para reduzir o sinal a 3,3 V.

### PZEM-004T v3 - comunicacao

| PZEM | NodeMCU V3 |
| --- | --- |
| 5V | 5 V regulados |
| GND | GND |
| TX | D6 / GPIO12 (RX do ESP8266) |
| RX | D7 / GPIO13 (TX do ESP8266) |

Use conversao de nivel logico adequada entre a UART de 5 V do PZEM e o ESP8266 de 3,3 V.

## Configuracao de Wi-Fi e API

1. Copie include/credenciais.h.example para include/credenciais.h no seu computador.
2. Edite o arquivo local include/credenciais.h e preencha WIFI_SSID e WIFI_PASSWORD. Esse arquivo real e ignorado pelo Git e nao deve ser publicado.
3. Abra include/config.h e troque HOMEWISE_API_URL pelo IP local do computador que executa a API, por exemplo http://192.168.1.25:3000/api/dados.
4. O ESP8266 e o computador que executa a API devem estar na mesma rede Wi-Fi de 2,4 GHz. Nao use localhost na URL do ESP8266.
5. Permita conexoes de entrada na porta 3000 no firewall do computador.

Para configurar banco, variaveis de ambiente e rotas, siga o [guia da API](api/README.md).

## Teste do firmware

1. Abra a pasta do repositorio que contem platformio.ini no VS Code.
2. Execute Clean, Build e Upload no ambiente nodemcuv2.
3. Abra o Monitor Serial em 115200 baud.
4. Teste primeiro o NodeMCU e o OLED.
5. Para simular um pulso sem o YF-S201, toque rapidamente D5 no GND.
6. Conecte e calibre o YF-S201 com um volume conhecido de agua.
7. Teste o PZEM por ultimo e confirme as leituras com cuidado.

Sem o PZEM respondendo, os valores eletricos podem permanecer na ultima leitura valida ou no valor inicial zero.

## Interface web

O dashboard esta na pasta [web](web). Foi criado com HTML, CSS e JavaScript puros e pode ser aberto diretamente em web/index.html ou com a extensao Live Server do VS Code. A interface inclui graficos, metas, alertas, historico e exportacao CSV. Os valores continuam demonstrativos ate a integracao do frontend com a API ser concluida.

## Seguranca eletrica

O lado de 127/220 V do PZEM nao deve ser montado em protoboard. A ligacao da rede depende da versao de 10 A ou 100 A do modulo e deve ser realizada com supervisao qualificada, isolamento e protecao adequados.

## Arquitetura

Sensor de vazao / PZEM -> ESP8266 -> Wi-Fi -> API REST -> MySQL -> Dashboard

## Proximas etapas

1. Executar o esquema SQL e configurar a API local.
2. Validar a conexao MySQL em GET /api/health.
3. Testar um POST para /api/dados e conferir a linha na tabela leituras.
4. Configurar a URL e as credenciais Wi-Fi no ESP8266.
5. Validar envio de leituras reais.
6. Substituir o dia acelerado por data/hora reais e definir a regra de agregacao diaria.
7. Conectar o dashboard aos dados reais da API.
