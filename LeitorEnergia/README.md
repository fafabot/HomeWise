# HomeWise — Monitor em Tempo Real (Energia + Água no Telão)

Aplicativo visual simplificado para projetar no telão durante a apresentação do TCC, exibindo **medida por medida** tanto o consumo de **Energia Elétrica (PZEM-004T v3.0)** quanto o de **Água (YF-S201)** enviados pelo WeMos D1 R1.

## Como abrir no dia da apresentação
- **Local:** dê dois cliques em **`Abrir_Leitor_Telao.bat`** (ou abra **`index.html`** no Chrome / Edge).
- **Pelo GitHub Pages:** acesse o link HTTPS publicado do repositório no Chrome / Edge.

## Modos de Visualização no Telão
Use as 3 abas no topo conforme o momento da demonstração:
- **`📊 Energia + Água`**: exibe os 8 cartões (4 de Energia + 4 de Água) e todas as medidas juntas.
- **`⚡ Focar Energia`**: oculta água, aumenta ainda mais os números de Tensão (`V`), Corrente (`A`), Potência (`W`) e Energia (`kWh`) e filtra a tabela só para Energia.
- **`💧 Focar Água`**: oculta energia, aumenta os números de Vazão (`L/min`), Volume em `mL`, Volume em `L` e `Pulsos`, e mostra cada leitura de 400ms do despejo de água entrando na tabela até a linha verde de **`CONSOLIDADO`**.
