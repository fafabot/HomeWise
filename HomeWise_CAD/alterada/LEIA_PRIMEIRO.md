# HOMEWISE — Pacote CAD Revisado (MDF 3 mm Parafusado + Suportes 3D Ajustados)

## O que foi corrigido nesta revisão (sobre o `HomeWise_CAD_preliminar`)
Todas as medidas foram ajustadas com base no hardware físico real medido e nas escolhas de montagem:

1. **Funil (`bico 6 mm` / `boca 55 mm`) — `#05 Clipe do Funil`:**
   - Centro do furo afastado de `10,5 mm` para **`38,0 mm` da face do painel** (deixa **`10,5 mm` de folga** livre atrás da boca de `55 mm`).
   - Diâmetro do furo ajustado de `14 mm` para **`Ø 6,8 mm`** para abraçar o bico de `6 mm`.
2. **Sensor `YF-S201` (`60 × 36 × 35 mm`) — `#06 Berço YF-S201`:**
   - Cavidade interna aprofundada para **`40 × 40 × 36,5 mm`** (antes tinha `23 mm` no STL / `15,5 mm` no SCAD, raso demais para os `35 mm` de espessura do sensor).
   - Berço elevado em pedestal (`16 mm`) para que o eixo do tubo fique em **`38,0 mm` da face do painel**, **coaxial com o funil** e alinhado sobre a área do copo `H2O` na base.
3. **WeMos D1 R1 (`#07`) — Colada no MDF:**
   - **Removidos** os 4 furos M4 do painel `.dxf` e da base 3D `#07` (fundo sólido liso para fita dupla-face VHB / cola).
   - Corrigido o 4º *standoff* M3 de `(60,5, 22,2)` para **`(61,8, 22,2)`** (gabarito oficial Uno/WeMos D1 R1).
   - Guias laterais ajustadas para **`54,2 mm`** de vão interno (acomoda a PCB de `53,4 mm`).
4. **Protoboard 400 pontos (`80 × 55 mm`) — Colada no MDF (sem OLED):**
   - Passa-cabos `Ø 10 mm` abertos de `X = 186 / 274` para **`X = 174` e `X = 286`** (`Y = 114 mm`), garantindo `11 mm` livres de cada lado da protoboard.
   - QR Code (`65 × 65 mm`) posicionado logo abaixo (`Y = 16..81 mm`).
5. **PZEM-004T v3 com Case Azul Transparente (`#08`) — Colado no MDF:**
   - **Removidos** os 2 furos M4 do painel `.dxf` e do fundo do case `#08` (fundo liso para colagem).
   - Rasgos em "U" do topo e da base alargados de `16 mm` para **`25 mm`** para não esmagar os 4 fios do borne verde (`~21 mm`).
   - Bloco reposicionado para centro **`(330, 200)`** (`X = 305..355`, `Y = 152..248`).
6. **TC 100A (`PZCT-02`, `47 × 31 × 29 mm`) — Movido para trás do painel:**
   - Removidos os furos frontais do TC no painel; toda a fiação AC 127/220V fica oculta atrás do painel.
   - **Recuo traseiro da base ampliado de `35 mm` para `55 mm`** (`02_base_totem_460x200.dxf`), dando folga confortável para o TC 100A e para os cabos da tomada.
7. **Tomada Schneider Electric Dexson Miluz (`75 × 73 × 48 mm`) — Horizontal, Parafusada:**
   - Reposicionada para **`(403, 214)`** — **100% acima da mão-francesa direita** (`Y > 175 mm`) e com `10,5 mm` de folga para o case do PZEM.
   - **2 furos M4 horizontais** separados por **`55 mm`**: `(375.5, 214)` e `(430.5, 214)`.
   - **Passa-cabo `Ø 12 mm`** deslocado **`17 mm` abaixo do centro** em `(403, 197)`, alinhado com o rasgo oblongo traseiro da tomada.
8. **Estrutura MDF 3 mm sem cola (Encaixe em "T" com Parafusos M3) + Gravação:**
   - **7 juntas em "T" (*T-slot*)** (`3×` Painel↔Base, `2×` Escoras↔Base, `2×` Escoras↔Painel) dimensionadas para **parafuso M3 × 16 mm + porca sextavada M3 (DIN 934)**.
   - Rasgos das escoras recuados para `X = 12..15` e `X = 445..448` (`12 mm` de margem lateral em vez de `6 mm`).
   - Removidos os 2 furos da base (copo apenas apoiado).
   - **5 pés niveladores (`09_pes_niveladores_m3_5x.stl`)** para esconder as cabeças dos 5 parafusos M3 sob a base.
   - Camada **`GRAVACAO`** adicionada aos DXFs (`HOMEWISE`, rótulos, caixas-guia de colagem e círculo `H2O` na base) e gravada em **baixo-relevo 3D** em `HomeWise_estrutura.stl` / `HomeWise_parametrico.scad`.

---

## Arquivos do Pacote

### A. Corte a Laser em MDF 3 mm (Camadas `CORTE` e `GRAVACAO`, escala 1:1 em mm)
- `01_painel_totem_460x340.dxf` (`460 × 340 mm`)
- `02_base_totem_460x200.dxf` (`460 × 200 mm`)
- `03_escora_vazada_esquerda.dxf` (`135 × 175 mm`)
- `04_escora_vazada_direita.dxf` (`135 × 175 mm`)

### B. Impressão 3D em PLA/PETG (Face traseira plana em `Z = 0`, zero suportes necessários)
- `05_suporte_3d_clipe_funil.stl` (`70 × 16 × 46 mm`) — furo `Ø 6,8 mm` a `38 mm` da parede
- `06_suporte_3d_berco_yfs201.stl` (`76 × 76 × 60,5 mm`) — cavidade `40 × 40 × 36,5 mm`, eixo a `38 mm`
- `07_suporte_3d_base_wemos_d1.stl` (`74 × 86 × 11,5 mm`) — colagem no MDF + 4 *standoffs* M3 corrigidos
- `08_suporte_3d_case_pzem004t.stl` (`50 × 96 × 25 mm`) — colagem no MDF + rasgos `25 mm`
- `09_pes_niveladores_m3_5x.stl` — 5 pés niveladores rebaixados para as cabeças M3 sob a base
- `10_adaptador_yfs201_mangueira_6mm_2x.stl` — 2 espigões rosca interna `G1/2"` (`YF-S201`) para mangueira `1/4"` (`~6 mm` interno), entrada + saída

### C. Montagem e Código-Fonte
- `HomeWise_parametrico.scad`
- `HomeWise_estrutura.stl` (montagem completa com gravação em baixo-relevo 3D)

---

## Lista de Ferragens (BOM) para Montagem
* **Estrutura MDF (7 juntas T-slot):**
  * `7×` Parafusos **M3 × 16 mm** (cabeça panela ou Allen)
  * `7×` Porcas sextavadas **M3** (DIN 934)
  * `2×` Arruelas **M3** (para os 2 parafusos das escoras atrás do painel; os 5 de baixo usam os pés `09_pes_niveladores_m3_5x.stl`)
* **Suportes de Água + Tomada (parafusados no painel):**
  * `8×` Parafusos **M4 × 10 mm** (ou **M4 × 12 mm**) com porca e arruela (`2×` Funil, `4×` YF-S201, `2×` Tomada Miluz)
* **WeMos D1 R1 nos *standoffs* do `#07`:**
  * `3×` ou `4×` Parafusos **M3 × 6 mm** (ou **M2,5** no canto próximo ao header digital)
* **Colagem no painel (use fita dupla-face VHB 3M ou cola adequada):**
  * Base `#07` da WeMos + Protoboard `80 × 55 mm` + Case `#08` do PZEM + Etiqueta QR Code + TC 100A atrás do painel.
