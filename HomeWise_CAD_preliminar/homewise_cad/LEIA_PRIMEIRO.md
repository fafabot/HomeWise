# HOMEWISE — Pacote CAD Totem MDF 3 mm + Suportes 3D (Fiel a `Render_Realista_Totem_MDF_3D_HomeWise.jpg`)

## Visão Geral
Este pacote CAD reproduz o modelo visual **`Render_Realista_Totem_MDF_3D_HomeWise.jpg`** (da pasta `diagramas/`), combinando **4 peças estruturais em MDF de 3 mm (Corte a Laser)** com **4 suportes dedicados para Impressão 3D (PLA)** e **fiação oculta por trás do painel**.

Todos os encaixes tipo dente (*finger-joints*) nos arquivos `.dxf`, `.scad` e `.stl` estão calibrados para **chapa de MDF de `3,0 mm` de espessura**.

---

## Coerência com as Peças Reais do HomeWise
Todas as cotas e furações foram dimensionadas a partir das medidas reais de paquímetro dos componentes do projeto:
1. **Placa WeMos D1 R1 (`68,6 x 53,4 mm`, formato Arduino Uno):**
   - A **Moldura 3D #07 (`74 x 86 mm`)** e a furação M3/M4 do painel foram dimensionadas para a sua **WeMos D1 R1 real**.
2. **Sensor de Vazão de Água YF-S201 1/2" (`60 x 42 x 36 mm`):**
   - Acomodado no **Berço 3D #06 (`76 x 76 mm`)** a `168 mm` de altura, com o **Clipe 3D #05** segurando o funil acima (`Y = 262 mm`) e `130 mm` livres abaixo para a mangueira desaguar no **Copo Coletor `H2O`** apoiado na base.
3. **Mini-Protoboard 170 furos (`47 x 35 mm`) + Etiqueta QR Code (`65 x 65 mm`):**
   - Posicionadas logo abaixo da WeMos D1 R1 (`Y = 118 mm` e `Y = 52 mm`), ladeadas por **4 furos passa-cabos redondos de $\varnothing 10\text{ mm}$** para os jumpers entrarem por trás do MDF exatamente como na foto.
4. **Medidor PZEM-004T v3.0 (`31 x 74 mm`) + Bobina TC 100A (`31 x 47 mm`) + Tomada Fêmea de Sobrepor (`46 x 82 mm`):**
   - O **Case 3D Laranja #08 (`50 x 96 x 24 mm`)** abriga o PZEM-004T v3.0 e protege os bornes energizados 127V/220V CA, ficando alinhado acima da **Bobina TC 100A** e ao lado da **Tomada Branca de Teste**.

---

## Arquivos do Pacote

### A. Corte a Laser em MDF 3 mm (Escala 1:1 em mm, Camada `CORTE`)
1. `01_painel_totem_460x340.dxf`: Painel frontal único (`460 x 340 x 3 mm`) com cantos superiores arredondados ($R=12\text{ mm}$), 4 espigas inferiores de `40 x 3 mm`, rasgos de `3,0 mm` para as mãos-francesas, furos M4/M3 dos suportes e 8 furos passa-cabos redondos.
2. `02_base_totem_460x180.dxf`: Base horizontal (`460 x 180 x 3 mm`) com os 8 rasgos de `3,0 mm` de largura para encaixe do Painel (`Y = 35..38 mm`) e das duas Mãos-Francesas (`X = 6..9 mm` e `X = 451..454 mm`).
3. `03_escora_vazada_esquerda.dxf`: Mão-francesa triangular esquerda (`135 x 175 x 3 mm`) com recorte triangular interno vazado e 4 espigas de `3,0 mm`.
4. `04_escora_vazada_direita.dxf`: Mão-francesa triangular direita (`135 x 175 x 3 mm`) com recorte triangular interno vazado e 4 espigas de `3,0 mm`.

### B. Impressão 3D em PLA (Escala 1:1 em mm — Face Traseira Plana em `Z = 0`)
5. `05_suporte_3d_clipe_funil.stl`: Braçadeira horizontal tipo Ômega ($\Omega$) azul (`70 x 16 x 22 mm`) com arco frontal e túnel **vertical** ($\varnothing 14\text{ mm}$ ao longo de `Y`) para a haste do funil + 2 abas laterais arredondadas com furos M4 escareados.
6. `06_suporte_3d_berco_yfs201.stl`: Berço azul (`76 x 76 x 24,5 mm`) com base de cantos arredondados ($R=7\text{ mm}$), 4 furos M4 escareados nos cantos e **duas garras laterais em formato de "C"** (com aba frontal de retenção e rasgo lateral direito para o cabo do sensor YF-S201).
7. `07_suporte_3d_base_wemos_d1.stl`: Placa-base **sólida** azul-escura (`74 x 86 x 10,5 mm`) com cantos arredondados ($R=6\text{ mm}$), 4 furos M4 escareados nos cantos, 4 torretas cilíndricas (*standoffs* M3) e guias laterais elevadas na face frontal para apoiar a placa WeMos D1 R1.
8. `08_suporte_3d_case_pzem004t.stl`: Case laranja (`50 x 96 x 22,5 mm`) com cantos arredondados ($R=5\text{ mm}$), fundo sólido parafusável (2 furos M4 ocultos sob a PCB), moldura interna rebaixada (degrau interno escuro) e **2 rasgos em "U" (no topo e na base)** para a saída dos fios dos bornes verdes do PZEM-004T v3.0.

### C. Visualização 3D e Documentação Técnica
- `HomeWise_estrutura.stl`: Montagem 3D completa (Totem em MDF 3 mm + os 4 suportes 3D posicionados no painel).
- `HomeWise_parametrico.scad`: Código-fonte paramétrico OpenSCAD (`T = 3`).
- `HomeWise_desenho_tecnico.pdf`: Desenho técnico de 2 páginas com Vista Frontal, Vista Lateral e tabela BOM completa para MDF 3 mm.
