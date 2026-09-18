# Dashboard HomeWise

Primeira versão da interface web do HomeWise. O site apresenta os dados de água e energia em um painel responsivo e funciona sem instalar dependências.

## Como abrir

Opção mais simples: abra o arquivo `index.html` no navegador.

Para executar com um servidor local no VS Code, também é possível usar a extensão Live Server e abrir a pasta `web`.

## O que já funciona

- Resumo do consumo diário de água e energia.
- Gráfico com seleção entre água e energia.
- Filtros de 7, 14 e 30 dias.
- Projeções e metas mensais.
- Indicação de consumo anormal acima de 20% da média anterior.
- Tabela com os registros recentes.
- Exportação do histórico de demonstração em CSV.
- Layout adaptado para computador, tablet e celular.
- Informações dos dispositivos usados no protótipo físico.

## Dados de demonstração

Enquanto a API ainda não está implementada, os valores exibidos ficam no arquivo `js/app.js`. A interface identifica esses dados como simulados para não confundi-los com medições reais.

## Integração planejada

Quando a API REST estiver pronta, os vetores `waterValues` e `energyValues` poderão ser substituídos por uma requisição HTTP. Um formato inicial de resposta pode ser:

```json
{
  "date": "2026-09-18",
  "waterLiters": 142,
  "energyKwh": 4.82,
  "voltage": 127.1,
  "current": 1.42,
  "power": 180.5,
  "abnormalWater": false,
  "abnormalEnergy": false
}
```

Fluxo final planejado:

`YF-S201 / PZEM-004T -> ESP8266 -> Wi-Fi -> API REST -> MySQL -> Dashboard`

## Estrutura

```text
web/
├── assets/
│   └── logo-homewise.svg
├── css/
│   └── styles.css
├── js/
│   └── app.js
├── index.html
└── README.md
```
