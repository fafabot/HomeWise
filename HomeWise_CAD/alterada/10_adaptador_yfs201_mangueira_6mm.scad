// HOMEWISE — Adaptador Espigao G1/2" (YF-S201) para Mangueira 1/4" (6 mm interno)
// Impresso em pe (Z=0 na base do sextavado), ZERO suportes necessarios!
// Recomendacao no fatiador: 3 a 4 paredes (perimetros), camada 0.16 ou 0.20 mm, passar fita veda-rosca na rosca do YF-S201.

$fn = 48;

module rosca_interna_g12(altura = 10.5, passo = 1.814, d_maior = 21.3, d_menor = 19.6) {
  // Rosca interna helicoidal G1/2" (14 FPP) com folga de 0.35 mm para impressao FDM
  voltas = altura / passo;
  passos_por_volta = 24;
  total_passos = ceil(voltas * passos_por_volta);
  dz = altura / total_passos;
  da = 360 / passos_por_volta;

  union() {
    cylinder(d = d_menor, h = altura);
    // Chanfro de entrada na base para facilitar o inicio da rosca
    cylinder(d1 = d_maior + 0.6, d2 = d_menor, h = 1.2);
    // Filete helicoidal por fatias finas rotacionadas (100% manifold no OpenSCAD)
    for (i = [0 : total_passos - 1]) {
      z0 = i * dz;
      ang = i * da;
      translate([0, 0, z0])
        rotate([0, 0, ang])
          linear_extrude(height = dz + 0.05, twist = -da, slices = 1)
            translate([(d_menor / 2) - 0.3, 0])
              circle(r = (d_maior - d_menor) / 2 + 0.3, $fn = 6);
    }
  }
}

module adaptador_yfs201_6mm() {
  difference() {
    union() {
      // 1. Corpo sextavado externo (25 mm entre faces, 11 mm de altura)
      cylinder(d = 25 / cos(30), h = 11.0, $fn = 6);

      // 2. Cone de transicao (45 graus, sem necessidade de suportes)
      translate([0, 0, 11.0])
        cylinder(d1 = 23.0, d2 = 10.0, h = 6.5);

      // 3. Espigao escalonado (barb) para mangueira 1/4" (~6 mm interno)
      translate([0, 0, 17.5]) cylinder(d = 6.1, h = 14.5);
      // 3 aneis de retencao (6.8 mm -> 6.1 mm)
      for (zb = [19.0, 23.5, 28.0]) {
        translate([0, 0, zb])
          cylinder(d1 = 6.8, d2 = 6.0, h = 3.2);
      }
      // Ponta chanfrada para facilitar encaixe da mangueira
      translate([0, 0, 31.2])
        cylinder(d1 = 6.1, d2 = 5.4, h = 1.0);
    }

    // Cavidade interna com rosca G1/2" (Z = -0.1 .. 10.5)
    translate([0, 0, -0.1])
      rosca_interna_g12(altura = 10.6);

    // Transicao interna conica (Z = 10.4 .. 17.5)
    translate([0, 0, 10.4])
      cylinder(d1 = 15.0, d2 = 4.0, h = 7.2);

    // Furo passante de agua no espigao (d = 4.0 mm)
    translate([0, 0, 17.4])
      cylinder(d = 4.0, h = 15.5);
  }
}

// 2 unidades lado a lado prontas para fatiar (entrada + saida do YF-S201)
translate([-18, 0, 0]) adaptador_yfs201_6mm();
translate([18, 0, 0]) adaptador_yfs201_6mm();
