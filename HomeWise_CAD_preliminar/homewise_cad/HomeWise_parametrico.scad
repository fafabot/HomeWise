// HOMEWISE — Totem Expositor em MDF 3 mm + 4 Suportes 3D Corrigidos (Fiel as 4 fotos de referencia)
T = 3;
modo = "montado"; // "montado" | "explodido"

module peca(n) {
  if (n == "01_painel_totem_460x340") linear_extrude(height=T) difference() { polygon(points=[[0,0],[45,0],[45,-3],[85,-3],[85,0],[165,0],[165,-3],[205,-3],[205,0],[255,0],[255,-3],[295,-3],[295,0],[375,0],[375,-3],[415,-3],[415,0],[460,0],[460,328],[459.77,330.34],[459.09,332.59],[457.98,334.67],[456.49,336.49],[454.67,337.98],[452.59,339.09],[450.34,339.77],[448,340],[12,340],[9.66,339.77],[7.41,339.09],[5.33,337.98],[3.51,336.49],[2.02,334.67],[0.91,332.59],[0.23,330.34],[0,328]]); translate([68,262]) circle(r=2.2,$fn=24); translate([122,262]) circle(r=2.2,$fn=24); translate([65,138]) circle(r=2.2,$fn=24); translate([125,138]) circle(r=2.2,$fn=24); translate([65,198]) circle(r=2.2,$fn=24); translate([125,198]) circle(r=2.2,$fn=24); translate([140,168]) circle(r=6,$fn=24); translate([199,161]) circle(r=2.2,$fn=24); translate([261,161]) circle(r=2.2,$fn=24); translate([199,235]) circle(r=2.2,$fn=24); translate([261,235]) circle(r=2.2,$fn=24); translate([178,198]) circle(r=5,$fn=24); translate([282,198]) circle(r=5,$fn=24); translate([186,118]) circle(r=5,$fn=24); translate([274,118]) circle(r=5,$fn=24); translate([342,168]) circle(r=2.2,$fn=24); translate([342,228]) circle(r=2.2,$fn=24); translate([342,258]) circle(r=5,$fn=24); translate([342,136]) circle(r=5,$fn=24); translate([324,92]) circle(r=2.2,$fn=24); translate([360,92]) circle(r=2.2,$fn=24); translate([342,52]) circle(r=5,$fn=24); translate([408,155]) circle(r=2.2,$fn=24); translate([408,215]) circle(r=2.2,$fn=24); translate([408,185]) circle(r=5,$fn=24); polygon(points=[[6,30],[9,30],[9,65],[6,65]]); polygon(points=[[6,115],[9,115],[9,150],[6,150]]); polygon(points=[[451,30],[454,30],[454,65],[451,65]]); polygon(points=[[451,115],[454,115],[454,150],[451,150]]); }
  if (n == "02_base_totem_460x180") linear_extrude(height=T) difference() { polygon(points=[[0,0],[460,0],[460,180],[0,180]]); translate([65,105]) circle(r=2.2,$fn=24); translate([125,105]) circle(r=2.2,$fn=24); polygon(points=[[45,35],[85,35],[85,38],[45,38]]); polygon(points=[[165,35],[205,35],[205,38],[165,38]]); polygon(points=[[255,35],[295,35],[295,38],[255,38]]); polygon(points=[[375,35],[415,35],[415,38],[375,38]]); polygon(points=[[6,60],[9,60],[9,95],[6,95]]); polygon(points=[[6,125],[9,125],[9,160],[6,160]]); polygon(points=[[451,60],[454,60],[454,95],[451,95]]); polygon(points=[[451,125],[454,125],[454,160],[451,160]]); }
  if (n == "03_escora_vazada_esquerda") linear_extrude(height=T) difference() { polygon(points=[[0,0],[22,0],[22,-3],[57,-3],[57,0],[87,0],[87,-3],[122,-3],[122,0],[135,0],[135,14],[14,175],[0,175],[0,150],[-3,150],[-3,115],[0,115],[0,65],[-3,65],[-3,30],[0,30]]); polygon(points=[[22,22],[96,22],[22,120]]); }
  if (n == "04_escora_vazada_direita") linear_extrude(height=T) difference() { polygon(points=[[0,0],[22,0],[22,-3],[57,-3],[57,0],[87,0],[87,-3],[122,-3],[122,0],[135,0],[135,14],[14,175],[0,175],[0,150],[-3,150],[-3,115],[0,115],[0,65],[-3,65],[-3,30],[0,30]]); polygon(points=[[22,22],[96,22],[22,120]]); }
  // #05 Clipe Tipo Omega para o Tubo Vertical do Funil (furo vertical ao longo de Y!)
  if (n == "05_suporte_3d_clipe_funil") union() {
    linear_extrude(height=4.5) difference() {
       hull() { translate([3.5,3.5]) circle(r=3.5,$fn=20); translate([19.5,3.5]) circle(r=3.5,$fn=20); translate([19.5,12.5]) circle(r=3.5,$fn=20); translate([3.5,12.5]) circle(r=3.5,$fn=20); }
      translate([8,8]) circle(r=2.2,$fn=20);
    }
    linear_extrude(height=4.5) difference() {
      hull() { translate([50.5,3.5]) circle(r=3.5,$fn=20); translate([66.5,3.5]) circle(r=3.5,$fn=20); translate([66.5,12.5]) circle(r=3.5,$fn=20); translate([50.5,12.5]) circle(r=3.5,$fn=20); }
      translate([62,8]) circle(r=2.2,$fn=20);
    }
    // Arco Omega com eixo vertical ao longo de Y (0..16)
    translate([0,16,0]) rotate([90,0,0]) linear_extrude(height=16) difference() {
      union() { square([30,4.5]); translate([20,0]) square([30,4.5]); translate([35,10.5]) circle(r=11.5,$fn=32); }
      translate([35,10.5]) circle(r=7.0,$fn=32);
      translate([0,-20]) square([70,20]);
    }
  }

  // #06 Berco com Garras em "C" para o Sensor YF-S201 (1/2")
  if (n == "06_suporte_3d_berco_yfs201") union() {
    linear_extrude(height=4.5) difference() {
      hull() { translate([7,7]) circle(r=7,$fn=24); translate([69,7]) circle(r=7,$fn=24); translate([69,69]) circle(r=7,$fn=24); translate([7,69]) circle(r=7,$fn=24); }
      translate([8,8]) circle(r=2.2,$fn=20); translate([68,8]) circle(r=2.2,$fn=20); translate([8,68]) circle(r=2.2,$fn=20); translate([68,68]) circle(r=2.2,$fn=20);
    }
    // Garras laterais em C com aba frontal de retencao e abertura lateral para o cabo
    translate([13,13,4.5]) cube([5.5,50,15.5]);
    translate([13,13,4.5]) cube([13,5,20]); translate([13,58,4.5]) cube([13,5,20]);
    translate([13,13,20]) cube([10.5,50,4.5]);
    translate([57.5,13,4.5]) cube([5.5,16,15.5]); translate([50,13,4.5]) cube([13,5,20]); translate([52.5,13,20]) cube([10.5,16,4.5]);
    translate([57.5,47,4.5]) cube([5.5,16,15.5]); translate([50,58,4.5]) cube([13,5,20]); translate([52.5,47,20]) cube([10.5,16,4.5]);
  }

  // #07 Placa-Base Solida Azul-Escura com Cantos Arredondados e 4 Standoffs M3 para WeMos D1 R1
  if (n == "07_suporte_3d_base_wemos_d1") union() {
    linear_extrude(height=4.5) difference() {
      hull() { translate([6,6]) circle(r=6,$fn=24); translate([68,6]) circle(r=6,$fn=24); translate([68,80]) circle(r=6,$fn=24); translate([6,80]) circle(r=6,$fn=24); }
      translate([6,6]) circle(r=2.2,$fn=20); translate([68,6]) circle(r=2.2,$fn=20); translate([6,80]) circle(r=2.2,$fn=20); translate([68,80]) circle(r=2.2,$fn=20);
    }
    for (pt = [[13.5,21.0],[18.6,73.0],[46.5,73.0],[60.5,22.2]])
      translate([pt[0],pt[1],4.5]) linear_extrude(height=6) difference() { circle(r=3.8,$fn=20); circle(r=1.6,$fn=16); }
    translate([8.5,12,4.5]) cube([3,64,3]); translate([62.5,12,4.5]) cube([3,64,3]);
  }

  // #08 Case Laranja do PZEM-004T v3.0 (berco duplo: acomoda o Case Azul Transparente 37x78x19 mm ou a PCB nua 30.5x73.5 mm)
  if (n == "08_suporte_3d_case_pzem004t") union() {
    linear_extrude(height=3.5) difference() {
      hull() { translate([5,5]) circle(r=5,$fn=24); translate([45,5]) circle(r=5,$fn=24); translate([45,91]) circle(r=5,$fn=24); translate([5,91]) circle(r=5,$fn=24); }
      translate([25,18]) circle(r=2.2,$fn=20); translate([25,78]) circle(r=2.2,$fn=20);
    }
    translate([0,0,3.5]) linear_extrude(height=2.5) difference() {
      hull() { translate([5,5]) circle(r=5,$fn=24); translate([45,5]) circle(r=5,$fn=24); translate([45,91]) circle(r=5,$fn=24); translate([5,91]) circle(r=5,$fn=24); }
      translate([9,11]) square([32,74]);
      translate([17,-1]) square([16,13]); translate([17,84]) square([16,13]);
    }
    translate([0,0,6.0]) linear_extrude(height=19.0) difference() {
      hull() { translate([5,5]) circle(r=5,$fn=24); translate([45,5]) circle(r=5,$fn=24); translate([45,91]) circle(r=5,$fn=24); translate([5,91]) circle(r=5,$fn=24); }
      translate([6,8.25]) square([38,79.5]);
      translate([17,-1]) square([16,10]); translate([17,87]) square([16,10]);
    }
  }
}

module posicionar_no_painel(ox, oz, e_off=0) {
  translate([ox, 38 + e_off, 3 + oz]) multmatrix([[1,0,0,0],[0,0,1,0],[0,1,0,0],[0,0,0,1]]) children();
}

module montagem(explodido=false) {
  e = explodido ? 50 : 0;
  color("#c89d6c") translate([0, 38 - e, 3 + e]) rotate([90, 0, 0]) peca("01_painel_totem_460x340");
  color("#b68b5b") translate([0, 0, 0]) peca("02_base_totem_460x180");
  color("#a37b4c") translate([6 - e, 38, 3]) rotate([90, 0, 90]) peca("03_escora_vazada_esquerda");
  color("#a37b4c") translate([451 + e, 38, 3]) rotate([90, 0, 90]) peca("04_escora_vazada_direita");
  color("#16a3b8") posicionar_no_painel(60, 254, e*0.6) peca("05_suporte_3d_clipe_funil");
  color("#16a3b8") posicionar_no_painel(57, 130, e*0.6) peca("06_suporte_3d_berco_yfs201");
  color("#1e3a8a") posicionar_no_painel(193, 155, e*0.6) peca("07_suporte_3d_base_wemos_d1");
  color("#ea580c") posicionar_no_painel(317, 150, e*0.6) peca("08_suporte_3d_case_pzem004t");
}

montagem(modo == "explodido");
