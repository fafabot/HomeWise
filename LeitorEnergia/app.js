// HomeWise — Monitor em Tempo Real (Energia + Água Medida por Medida para Telão)
// Compatível com:
// 1. Porta Serial USB direta (Web Serial API @ 115200 bps — lê [ENERGIA] e [AGUA] do WeMos D1 R1)
// 2. Nuvem Firebase Firestore em tempo real (coleção "leituras", central_homewise_01)
// 3. Simuladores de Energia e de Despejo de Água para demonstração no telão

const FIREBASE_PROJECT_ID = 'homewise-65bb1';
const CENTRAL_PADRAO = 'central_homewise_01';
const PULSOS_POR_LITRO = 450.0;

// Elementos gerais
const statusBadge = document.getElementById('statusBadge');
const statusText = document.getElementById('statusText');
const pillOrigem = document.getElementById('pillOrigem');
const pillContador = document.getElementById('pillContador');
const txtUltimaHora = document.getElementById('txtUltimaHora');
const tabelaMedidas = document.getElementById('tabelaMedidas');
const consoleBruto = document.getElementById('consoleBruto');

const blocoEnergia = document.getElementById('blocoEnergia');
const blocoAgua = document.getElementById('blocoAgua');

// Cartões de Energia
const pillCarga = document.getElementById('pillCarga');
const valTensao = document.getElementById('valTensao');
const valCorrente = document.getElementById('valCorrente');
const valPotencia = document.getElementById('valPotencia');
const valEnergia = document.getElementById('valEnergia');
const footTensao = document.getElementById('footTensao');
const footCorrente = document.getElementById('footCorrente');
const footPotencia = document.getElementById('footPotencia');
const footEnergia = document.getElementById('footEnergia');

// Cartões de Água
const pillFluxoAgua = document.getElementById('pillFluxoAgua');
const valVazaoAgua = document.getElementById('valVazaoAgua');
const valVolumeMl = document.getElementById('valVolumeMl');
const valVolumeLitros = document.getElementById('valVolumeLitros');
const valPulsosAgua = document.getElementById('valPulsosAgua');
const footVazaoAgua = document.getElementById('footVazaoAgua');
const footVolumeMl = document.getElementById('footVolumeMl');
const footVolumeLitros = document.getElementById('footVolumeLitros');
const footPulsosAgua = document.getElementById('footPulsosAgua');

// Botões
const btnConectarSerial = document.getElementById('btnConectarSerial');
const btnModoFirebase = document.getElementById('btnModoFirebase');
const btnSimular = document.getElementById('btnSimular');
const btnSimularAgua = document.getElementById('btnSimularAgua');
const btnTelaCheia = document.getElementById('btnTelaCheia');
const btnLerAgora = document.getElementById('btnLerAgora');
const btnZerarESP = document.getElementById('btnZerarESP');
const btnPausar = document.getElementById('btnPausar');
const btnLimpar = document.getElementById('btnLimpar');
const tabButtons = document.querySelectorAll('.tab-btn');

// Estado da sessão
let modoVisao = 'ambos'; // 'ambos' | 'energia' | 'agua'
let contadorMedidas = 0;
let picoPotenciaW = 0;
let picoVazaoLMin = 0;
let listaPausada = false;

// Controle de tara (offset apenas na Web, sem apagar a medição diária do ESP/Firebase)
let temLeituraEnergia = false;
let temLeituraAgua = false;
let ultimoBrutoEnergiaKwh = 0;
let ultimoBrutoLitros = 0;
let ultimoBrutoMl = 0;
let ultimoBrutoPulsos = 0;
let taraEnergiaKwh = 0;
let taraLitros = 0;
let taraMl = 0;
let taraPulsos = 0;
let zerarNaProximaEnergia = false;
let zerarNaProximaAgua = false;

// Conexões e timers
let portaSerial = null;
let leitorSerial = null;
let timerFirebase = null;
let ultimoIdFirestore = null;
let timerSimEnergia = null;
let timerSimAgua = null;
let energiaSimuladaKwh = 0.012;
let pulsosSimulados = 0;

// Buffers multilinha do Serial (relatório manual 'p' e relatório consolidado de água)
let blocoManualEnergia = {};
let blocoRelatorioAgua = {};

function formatarNumero(valor, casas) {
  return Number(valor || 0).toFixed(casas).replace('.', ',');
}

function horarioFormatado(data = new Date()) {
  return data.toLocaleTimeString('pt-BR', {
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit'
  });
}

function atualizarStatus(tipo, texto, origem) {
  statusBadge.className = `status-badge ${tipo}`;
  statusText.textContent = texto;
  if (origem) {
    pillOrigem.textContent = `Origem: ${origem}`;
  }
}

function anexarConsoleBruto(linha) {
  const textoAtual = consoleBruto.textContent === 'Aguardando dados da porta serial...'
    ? ''
    : consoleBruto.textContent;
  const linhas = (textoAtual + '\n' + linha).trim().split('\n').slice(-100);
  consoleBruto.textContent = linhas.join('\n');
  consoleBruto.scrollTop = consoleBruto.scrollHeight;
}

function aplicarFiltroTabela() {
  const linhas = tabelaMedidas.querySelectorAll('tr[data-recurso]');
  linhas.forEach((tr) => {
    const rec = tr.getAttribute('data-recurso');
    if (modoVisao === 'ambos' || modoVisao === rec) {
      tr.classList.remove('hidden');
    } else {
      tr.classList.add('hidden');
    }
  });
}

function inserirLinhaTabela({ recurso, leituraPrincipal, leituraSecundaria, acumulado, estadoTexto, estadoClasse, horaStr, consolidado = false }) {
  contadorMedidas += 1;
  pillContador.textContent = `${contadorMedidas} ${contadorMedidas === 1 ? 'medida recebida' : 'medidas recebidas'}`;
  txtUltimaHora.textContent = `Última leitura: ${horaStr}`;

  if (listaPausada) return;

  const linhaVazia = tabelaMedidas.querySelector('.empty-row');
  if (linhaVazia) linhaVazia.remove();

  const tr = document.createElement('tr');
  tr.setAttribute('data-recurso', recurso);
  if (consolidado) tr.classList.add('row-consolidado');
  if (modoVisao !== 'ambos' && modoVisao !== recurso) {
    tr.classList.add('hidden');
  }

  const tagHtml = recurso === 'energia'
    ? '<span class="tag-recurso recurso-energia">⚡ ENERGIA</span>'
    : '<span class="tag-recurso recurso-agua">💧 ÁGUA</span>';

  tr.innerHTML = `
    <td>#${contadorMedidas}</td>
    <td>${horaStr}</td>
    <td>${tagHtml}</td>
    <td>${leituraPrincipal}</td>
    <td>${leituraSecundaria}</td>
    <td>${acumulado}</td>
    <td><span class="badge-estado ${estadoClasse}">${estadoTexto}</span></td>
  `;

  tabelaMedidas.prepend(tr);

  while (tabelaMedidas.children.length > 80) {
    tabelaMedidas.removeChild(tabelaMedidas.lastElementChild);
  }
}

function pulsarCartoes(seletor, classe) {
  document.querySelectorAll(seletor).forEach((card) => {
    card.classList.add(classe);
    setTimeout(() => card.classList.remove(classe), 320);
  });
}

// Registra uma nova medida de ENERGIA
function registrarMedidaEnergia({ tensaoV, correnteA, potenciaW, energiaKwh, dataHora }) {
  const t = Number(tensaoV || 0);
  const p = Number(potenciaW || 0);
  const c = correnteA !== undefined && correnteA !== null
    ? Number(correnteA)
    : (t > 0 ? p / t : 0);
  const brutoE = Number(energiaKwh || 0);

  if (zerarNaProximaEnergia) {
    taraEnergiaKwh = brutoE;
    zerarNaProximaEnergia = false;
  } else if (brutoE < taraEnergiaKwh - 1e-6) {
    // Caso o ESP8266 tenha reiniciado por conta própria
    taraEnergiaKwh = 0;
  }

  ultimoBrutoEnergiaKwh = brutoE;
  temLeituraEnergia = true;

  const e = Math.max(0, brutoE - taraEnergiaKwh);
  const horaStr = horarioFormatado(dataHora || new Date());

  if (p > picoPotenciaW) picoPotenciaW = p;

  valTensao.textContent = formatarNumero(t, 1);
  valCorrente.textContent = formatarNumero(c, 2);
  valPotencia.textContent = formatarNumero(p, 1);
  valEnergia.textContent = formatarNumero(e, 4);

  if (t <= 0) {
    footTensao.textContent = 'Sem tensão AC detectada';
  } else if (t >= 110 && t <= 138) {
    footTensao.textContent = 'Rede 127 V nominal estável';
  } else if (t >= 200 && t <= 240) {
    footTensao.textContent = 'Rede 220 V nominal estável';
  } else {
    footTensao.textContent = 'Atenção: tensão fora da faixa padrão';
  }

  const cargaAtiva = p > 1.0;
  footCorrente.textContent = cargaAtiva ? 'Circuito com carga ativa' : 'Corrente de repouso';
  footPotencia.textContent = `Pico na sessão: ${formatarNumero(picoPotenciaW, 1)} W`;
  footEnergia.textContent = taraEnergiaKwh > 0
    ? `Zerado na web • Dia: ${formatarNumero(brutoE, 4)} kWh`
    : `Atualizado às ${horaStr}`;

  pillCarga.textContent = cargaAtiva ? 'Carga: LIGADA' : 'Carga: Em espera';
  pillCarga.style.color = cargaAtiva ? '#fbbf24' : '#4ade80';

  pulsarCartoes('#blocoEnergia .metric-card', 'pulse-energy');

  inserirLinhaTabela({
    recurso: 'energia',
    leituraPrincipal: `${formatarNumero(p, 1)} W  (${formatarNumero(c, 2)} A)`,
    leituraSecundaria: `${formatarNumero(t, 1)} V`,
    acumulado: `${formatarNumero(e, 4)} kWh`,
    estadoTexto: cargaAtiva ? 'CARGA LIGADA' : 'EM ESPERA',
    estadoClasse: cargaAtiva ? 'badge-ativa' : 'badge-espera',
    horaStr
  });
}

// Registra uma nova medida de ÁGUA (em fluxo a cada 400ms ou relatório final)
function registrarMedidaAgua({ vazaoLMin, volumeMl, volumeLitros, pulsos, dataHora, consolidado = false }) {
  const q = Number(vazaoLMin || 0);
  const brutoMl = volumeMl !== undefined
    ? Number(volumeMl)
    : Number(volumeLitros || 0) * 1000;
  const brutoLitros = volumeLitros !== undefined
    ? Number(volumeLitros)
    : brutoMl / 1000;
  const brutoPul = pulsos !== undefined
    ? Math.round(Number(pulsos))
    : Math.round(brutoLitros * PULSOS_POR_LITRO);

  if (zerarNaProximaAgua) {
    taraMl = brutoMl;
    taraLitros = brutoLitros;
    taraPulsos = brutoPul;
    zerarNaProximaAgua = false;
  } else if (brutoMl < taraMl - 0.05) {
    // Caso o ESP8266 tenha reiniciado por conta própria
    taraMl = 0;
    taraLitros = 0;
    taraPulsos = 0;
  }

  ultimoBrutoMl = brutoMl;
  ultimoBrutoLitros = brutoLitros;
  ultimoBrutoPulsos = brutoPul;
  temLeituraAgua = true;

  const ml = Math.max(0, brutoMl - taraMl);
  const litros = Math.max(0, brutoLitros - taraLitros);
  const pul = Math.max(0, brutoPul - taraPulsos);
  const horaStr = horarioFormatado(dataHora || new Date());

  if (q > picoVazaoLMin) picoVazaoLMin = q;

  valVazaoAgua.textContent = formatarNumero(consolidado ? 0 : q, 2);
  valVolumeMl.textContent = formatarNumero(ml, 1);
  valVolumeLitros.textContent = formatarNumero(litros, 3);
  valPulsosAgua.textContent = String(pul);

  footVazaoAgua.textContent = `Pico de vazão: ${formatarNumero(picoVazaoLMin, 2)} L/min`;
  footVolumeMl.textContent = consolidado ? 'Despejo finalizado e consolidado!' : `Atualizado às ${horaStr}`;
  footVolumeLitros.textContent = taraLitros > 0
    ? `Zerado na web • Dia: ${formatarNumero(brutoLitros, 3)} L`
    : `${formatarNumero(ml, 1)} mL medidos`;
  footPulsosAgua.textContent = `Fator: 450,0 pulsos/L`;

  const emFluxo = !consolidado && q > 0.05;
  pillFluxoAgua.textContent = consolidado
    ? 'Fluxo: Despejo concluído'
    : (emFluxo ? 'Fluxo: PASSANDO ÁGUA' : 'Fluxo: Parado');
  pillFluxoAgua.style.color = emFluxo ? '#38bdf8' : '#4ade80';

  pulsarCartoes('#blocoAgua .metric-card', 'pulse-water');

  inserirLinhaTabela({
    recurso: 'agua',
    leituraPrincipal: consolidado ? 'Fim do despejo (0,00 L/min)' : `${formatarNumero(q, 2)} L/min`,
    leituraSecundaria: `${pul} pulsos`,
    acumulado: `${formatarNumero(ml, 1)} mL  (${formatarNumero(litros, 3)} L)`,
    estadoTexto: consolidado ? 'CONSOLIDADO' : (emFluxo ? 'FLUXO ATIVO' : 'SEM FLUXO'),
    estadoClasse: consolidado ? 'badge-espera' : (emFluxo ? 'badge-agua-fluxo' : 'badge-espera'),
    horaStr,
    consolidado
  });
}

function zerarVisorLocal() {
  picoPotenciaW = 0;
  picoVazaoLMin = 0;

  valEnergia.textContent = '0,0000';
  valVazaoAgua.textContent = '0,00';
  valVolumeMl.textContent = '0,0';
  valVolumeLitros.textContent = '0,000';
  valPulsosAgua.textContent = '0';

  footPotencia.textContent = 'Pico na sessão: 0,0 W';
  footEnergia.textContent = temLeituraEnergia
    ? `Zerado na web • Dia: ${formatarNumero(ultimoBrutoEnergiaKwh, 4)} kWh`
    : 'Zerado na web (aguardando leitura)';
  footVazaoAgua.textContent = 'Pico de vazão: 0,00 L/min';
  footVolumeMl.textContent = 'Visor zerado nesta sessão web';
  footVolumeLitros.textContent = temLeituraAgua
    ? `Zerado na web • Dia: ${formatarNumero(ultimoBrutoLitros, 3)} L`
    : 'Zerado na web (aguardando leitura)';

  pillFluxoAgua.textContent = 'Fluxo: Zerado na Web';
  pillFluxoAgua.style.color = '#4ade80';

  pulsarCartoes('.metric-card', 'pulse-water');
}

// Zera APENAS a visualização web (tara local), preservando a medição do dia no ESP8266 e no Firebase
function zerarAcumuladoWeb() {
  if (timerSimEnergia || timerSimAgua) {
    energiaSimuladaKwh = 0;
    pulsosSimulados = 0;
    ultimoBrutoEnergiaKwh = 0;
    ultimoBrutoLitros = 0;
    ultimoBrutoMl = 0;
    ultimoBrutoPulsos = 0;
    taraEnergiaKwh = 0;
    taraLitros = 0;
    taraMl = 0;
    taraPulsos = 0;
    zerarNaProximaEnergia = false;
    zerarNaProximaAgua = false;
  } else {
    if (temLeituraEnergia) {
      taraEnergiaKwh = ultimoBrutoEnergiaKwh;
      zerarNaProximaEnergia = false;
    } else {
      zerarNaProximaEnergia = true;
    }

    if (temLeituraAgua) {
      taraLitros = ultimoBrutoLitros;
      taraMl = ultimoBrutoMl;
      taraPulsos = ultimoBrutoPulsos;
      zerarNaProximaAgua = false;
    } else {
      zerarNaProximaAgua = true;
    }

    energiaSimuladaKwh = 0;
    pulsosSimulados = 0;
  }

  zerarVisorLocal();
}

// Interpretador das linhas enviadas pelo firmware HomeWise (src/main.cpp)
function processarLinhaSerial(linhaBruta) {
  const linha = linhaBruta.trim();
  if (!linha) return;

  anexarConsoleBruto(linha);

  // 0. Caso alguém tenha zerado diretamente no hardware ('r' pelo terminal externo)
  if (linha.includes('[ZERADO]')) {
    ultimoBrutoEnergiaKwh = 0;
    ultimoBrutoLitros = 0;
    ultimoBrutoMl = 0;
    ultimoBrutoPulsos = 0;
    taraEnergiaKwh = 0;
    taraLitros = 0;
    taraMl = 0;
    taraPulsos = 0;
    zerarVisorLocal();
    return;
  }

  // 1. Início de despejo de água
  if (linha.includes('[AGUA - INICIO]')) {
    pillFluxoAgua.textContent = 'Fluxo: INICIANDO DESPEJO...';
    pillFluxoAgua.style.color = '#38bdf8';
    return;
  }

  // 2. Linha de fluxo de água a cada 400ms:
  // -> [AGUA] Pulsos: 120  |  Volume: 298.1 mL  |  Vazao: 3.45 L/min
  const matchAgua = linha.match(
    /\[AGUA\]\s*Pulsos:\s*(\d+)\s*\|\s*Volume:\s*([\d.]+)\s*mL\s*\|\s*Vazao:\s*([\d.]+)\s*L\/min/i
  );
  if (matchAgua) {
    atualizarStatus('online', 'Recebendo Água (YF-S201)', 'USB Serial (115200 bps)');
    registrarMedidaAgua({
      pulsos: parseInt(matchAgua[1], 10),
      volumeMl: parseFloat(matchAgua[2]),
      vazaoLMin: parseFloat(matchAgua[3])
    });
    return;
  }

  // 3. Relatório consolidado de água ao encerrar despejo (após 2s sem pulsos):
  // Total de Pulsos : 161 pulsos
  // Volume Medido   : 400.0 mL  (0.400 L)
  const mTotalPulsos = linha.match(/^Total de Pulsos\s*:\s*(\d+)\s*pulsos/i);
  if (mTotalPulsos) {
    blocoRelatorioAgua.pulsos = parseInt(mTotalPulsos[1], 10);
    return;
  }
  const mVolumeConsolidado = linha.match(/^Volume Medido\s*:\s*([\d.]+)\s*mL\s*\(([\d.]+)\s*L\)/i);
  if (mVolumeConsolidado) {
    registrarMedidaAgua({
      vazaoLMin: 0,
      volumeMl: parseFloat(mVolumeConsolidado[1]),
      volumeLitros: parseFloat(mVolumeConsolidado[2]),
      pulsos: blocoRelatorioAgua.pulsos,
      consolidado: true
    });
    blocoRelatorioAgua = {};
    return;
  }

  // 4. Linha periódica de energia a cada 2s:
  // [ENERGIA] Tensao: 127.3 V  |  Corrente: 0.45 A  |  Potencia: 57.3 W  |  Energia: 0.0124 kWh
  const matchEnergia = linha.match(
    /\[ENERGIA\]\s*Tensao:\s*([\d.]+)\s*V\s*\|\s*Corrente:\s*([\d.]+)\s*A\s*\|\s*Potencia:\s*([\d.]+)\s*W\s*\|\s*Energia:\s*([\d.]+)\s*kWh/i
  );
  if (matchEnergia) {
    atualizarStatus('online', 'Monitorando Sensores', 'USB Serial (115200 bps)');
    registrarMedidaEnergia({
      tensaoV: parseFloat(matchEnergia[1]),
      correnteA: parseFloat(matchEnergia[2]),
      potenciaW: parseFloat(matchEnergia[3]),
      energiaKwh: parseFloat(matchEnergia[4])
    });
    return;
  }

  // 5. Aviso de PZEM aguardando tomada AC:
  if (linha.includes('Aguardando PZEM') || linha.includes('SEM COMUNICACAO COM O PZEM')) {
    atualizarStatus('waiting', 'Água Ativa • PZEM sem tomada AC', 'USB Serial (115200 bps)');
    footTensao.textContent = 'Conecte o PZEM na tomada AC 127/220V';
    return;
  }

  // 6. Leitura manual ('p') ou bloco de energia do relatório consolidado:
  const mTensao = linha.match(/^Tensao(?:\s+da\s+Rede)?\s*:\s*([\d.]+)\s*V/i);
  if (mTensao) {
    blocoManualEnergia.tensaoV = parseFloat(mTensao[1]);
    return;
  }
  const mCorrente = linha.match(/^Corrente\s*:\s*([\d.]+)\s*A/i);
  if (mCorrente) {
    blocoManualEnergia.correnteA = parseFloat(mCorrente[1]);
    return;
  }
  const mPotencia = linha.match(/^Potencia(?:\s+Ativa)?\s*:\s*([\d.]+)\s*W/i);
  if (mPotencia) {
    blocoManualEnergia.potenciaW = parseFloat(mPotencia[1]);
    return;
  }
  const mEnergia = linha.match(/^Energia\s*:\s*([\d.]+)\s*kWh/i);
  if (mEnergia && blocoManualEnergia.tensaoV !== undefined) {
    blocoManualEnergia.energiaKwh = parseFloat(mEnergia[1]);
    atualizarStatus('online', 'Leitura PZEM Recebida', 'USB Serial (115200 bps)');
    registrarMedidaEnergia(blocoManualEnergia);
    blocoManualEnergia = {};
  }
}

async function pararSimulacoes() {
  if (timerSimEnergia) {
    clearInterval(timerSimEnergia);
    timerSimEnergia = null;
    btnSimular.textContent = '⚡ Simular Energia';
  }
  if (timerSimAgua) {
    clearInterval(timerSimAgua);
    timerSimAgua = null;
    btnSimularAgua.textContent = '💧 Simular Despejo';
  }
}

async function pararModosAtivos() {
  await pararSimulacoes();
  if (timerFirebase) {
    clearInterval(timerFirebase);
    timerFirebase = null;
    btnModoFirebase.textContent = '☁️ Receber via Firebase';
  }
  if (leitorSerial) {
    try {
      await leitorSerial.cancel();
    } catch {}
    leitorSerial = null;
  }
  if (portaSerial) {
    try {
      await portaSerial.close();
    } catch {}
    portaSerial = null;
  }
  btnLerAgora.disabled = true;
}

function resetarBaseAoTrocarFonte() {
  temLeituraEnergia = false;
  temLeituraAgua = false;
  ultimoBrutoEnergiaKwh = 0;
  ultimoBrutoLitros = 0;
  ultimoBrutoMl = 0;
  ultimoBrutoPulsos = 0;
  taraEnergiaKwh = 0;
  taraLitros = 0;
  taraMl = 0;
  taraPulsos = 0;
}

// MODO 1: Conexão USB Serial Direta (Web Serial API)
async function conectarUSBSerial() {
  if (portaSerial) {
    await pararModosAtivos();
    atualizarStatus('offline', 'Desconectado', 'Aguardando conexão');
    btnConectarSerial.textContent = '🔌 Conectar USB (Serial)';
    return;
  }

  if (!('serial' in navigator)) {
    alert('Seu navegador não suporta Web Serial API. Abra no Google Chrome ou Microsoft Edge.');
    return;
  }

  try {
    await pararModosAtivos();
    resetarBaseAoTrocarFonte();
    portaSerial = await navigator.serial.requestPort();
    await portaSerial.open({ baudRate: 115200 });

    btnConectarSerial.textContent = '⏹ Desconectar USB';
    btnLerAgora.disabled = false;
    atualizarStatus('online', 'USB Conectado (115200 bps)', 'USB Serial (WeMos D1 R1)');

    const decoder = new TextDecoderStream();
    portaSerial.readable.pipeTo(decoder.writable).catch(() => {});
    leitorSerial = decoder.readable.getReader();

    let acumulado = '';
    while (true) {
      const { value, done } = await leitorSerial.read();
      if (done) break;
      if (value) {
        acumulado += value;
        const partes = acumulado.split(/\r?\n/);
        acumulado = partes.pop() || '';
        for (const linha of partes) {
          processarLinhaSerial(linha);
        }
      }
    }
  } catch (err) {
    if (err?.name !== 'NotFoundError') {
      alert('Não foi possível abrir a porta serial. Feche o Monitor Serial do VS Code/Arduino antes de conectar aqui.');
    }
    await pararModosAtivos();
    btnConectarSerial.textContent = '🔌 Conectar USB (Serial)';
    atualizarStatus('offline', 'Desconectado', 'Aguardando conexão');
  }
}

async function enviarComandoSerial(caractere) {
  if (!portaSerial || !portaSerial.writable) return;
  const writer = portaSerial.writable.getWriter();
  try {
    await writer.write(new TextEncoder().encode(caractere + '\n'));
  } finally {
    writer.releaseLock();
  }
}

// MODO 2: Receber Medida por Medida via Firebase Firestore REST API
async function consultarUltimaMedidaFirestore() {
  const url = `https://firestore.googleapis.com/v1/projects/${FIREBASE_PROJECT_ID}/databases/(default)/documents:runQuery`;
  const body = {
    structuredQuery: {
      from: [{ collectionId: 'leituras' }],
      orderBy: [{ field: { fieldPath: 'criado_em' }, direction: 'DESCENDING' }],
      limit: 5
    }
  };

  const resp = await fetch(url, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json' },
    body: JSON.stringify(body)
  });
  if (!resp.ok) throw new Error(`HTTP ${resp.status}`);
  const rows = await resp.json();

  const docs = rows
    .filter((r) => r.document)
    .map((r) => {
      const f = r.document.fields || {};
      const id = r.document.name.split('/').pop();
      const num = (campo) => Number(f[campo]?.doubleValue ?? f[campo]?.integerValue ?? 0);
      return {
        id,
        dispositivo_id: f.dispositivo_id?.stringValue || CENTRAL_PADRAO,
        tensaoV: num('tensao_v'),
        potenciaW: num('potencia_w'),
        energiaKwh: num('energia_kwh'),
        vazaoLMin: num('vazao_l_min'),
        consumoAguaLitros: num('consumo_agua_litros'),
        criadoEm: f.criado_em?.timestampValue ? new Date(f.criado_em.timestampValue) : new Date()
      };
    })
    .filter((d) => d.dispositivo_id === CENTRAL_PADRAO);

  if (docs.length === 0) return;

  const maisRecente = docs[0];
  if (maisRecente.id !== ultimoIdFirestore) {
    ultimoIdFirestore = maisRecente.id;
    registrarMedidaEnergia({
      tensaoV: maisRecente.tensaoV,
      potenciaW: maisRecente.potenciaW,
      energiaKwh: maisRecente.energiaKwh,
      dataHora: maisRecente.criadoEm
    });
    registrarMedidaAgua({
      vazaoLMin: maisRecente.vazaoLMin,
      volumeLitros: maisRecente.consumoAguaLitros,
      dataHora: maisRecente.criadoEm
    });
  }
}

async function alternarModoFirebase() {
  if (timerFirebase) {
    await pararModosAtivos();
    atualizarStatus('offline', 'Desconectado', 'Aguardando conexão');
    return;
  }

  await pararModosAtivos();
  resetarBaseAoTrocarFonte();
  btnConectarSerial.textContent = '🔌 Conectar USB (Serial)';
  btnModoFirebase.textContent = '⏹ Parar Firebase';
  atualizarStatus('online', 'Sincronizado ao Firestore', `Firebase (${CENTRAL_PADRAO})`);
  ultimoIdFirestore = null;

  try {
    await consultarUltimaMedidaFirestore();
  } catch (e) {
    console.warn('Erro ao consultar Firestore:', e);
  }

  timerFirebase = setInterval(() => {
    consultarUltimaMedidaFirestore().catch(() => {});
  }, 3000);
}

// MODO 3A: Simulação de Energia Contínua
async function alternarSimulacaoEnergia() {
  if (timerSimEnergia) {
    clearInterval(timerSimEnergia);
    timerSimEnergia = null;
    btnSimular.textContent = '⚡ Simular Energia';
    if (!timerSimAgua) atualizarStatus('offline', 'Simulação pausada', 'Aguardando conexão');
    return;
  }

  if (portaSerial || timerFirebase) {
    await pararModosAtivos();
    resetarBaseAoTrocarFonte();
  }
  btnConectarSerial.textContent = '🔌 Conectar USB (Serial)';
  btnSimular.textContent = '⏹ Parar Energia';
  atualizarStatus('online', 'Simulando Sensores no Telão', 'Modo Demonstração');

  const gerarPassoEnergia = () => {
    const tensao = 126.8 + (Math.random() * 1.4 - 0.7);
    const potencia = 65 + Math.random() * 15;
    const corrente = potencia / tensao;
    energiaSimuladaKwh += (potencia / 1000) * (2 / 3600);
    processarLinhaSerial(
      `[ENERGIA] Tensao: ${tensao.toFixed(1)} V  |  Corrente: ${corrente.toFixed(2)} A  |  Potencia: ${potencia.toFixed(1)} W  |  Energia: ${energiaSimuladaKwh.toFixed(4)} kWh`
    );
  };

  gerarPassoEnergia();
  timerSimEnergia = setInterval(gerarPassoEnergia, 2000);
}

// MODO 3B: Simulação de um Despejo de Água (ex.: copo/garrafa de ~500 mL medida por medida a cada 400ms)
async function simularDespejoAgua() {
  if (timerSimAgua) {
    clearInterval(timerSimAgua);
    timerSimAgua = null;
    btnSimularAgua.textContent = '💧 Simular Despejo';
    return;
  }

  if (portaSerial || timerFirebase) {
    await pararModosAtivos();
    resetarBaseAoTrocarFonte();
  }
  btnSimularAgua.textContent = '⏹ Parar Despejo';
  atualizarStatus('online', 'Simulando Despejo de Água', 'Modo Demonstração');

  processarLinhaSerial('>> [AGUA - INICIO] Fluxo detectado! Despejando...');
  let etapas = 0;
  const totalEtapas = 10; // ~4 segundos despejando ~500 mL (201 pulsos)

  timerSimAgua = setInterval(() => {
    etapas += 1;
    const incrementoPulsos = Math.round(19 + Math.random() * 3);
    pulsosSimulados += incrementoPulsos;
    const litros = pulsosSimulados / PULSOS_POR_LITRO;
    const ml = litros * 1000;
    const vazao = (incrementoPulsos / PULSOS_POR_LITRO) * (60000 / 400);

    processarLinhaSerial(
      `   -> [AGUA] Pulsos: ${pulsosSimulados}  |  Volume: ${ml.toFixed(1)} mL  |  Vazao: ${vazao.toFixed(2)} L/min`
    );

    if (etapas >= totalEtapas) {
      clearInterval(timerSimAgua);
      timerSimAgua = null;
      btnSimularAgua.textContent = '💧 Simular Despejo';
      setTimeout(() => {
        const litrosFinal = pulsosSimulados / PULSOS_POR_LITRO;
        processarLinhaSerial(`Total de Pulsos : ${pulsosSimulados} pulsos`);
        processarLinhaSerial(`Volume Medido   : ${(litrosFinal * 1000).toFixed(1)} mL  (${litrosFinal.toFixed(3)} L)`);
      }, 900);
    }
  }, 450);
}

// Abas de foco no telão (Energia + Água | Só Energia | Só Água)
tabButtons.forEach((btn) => {
  btn.addEventListener('click', () => {
    tabButtons.forEach((b) => b.classList.remove('active'));
    btn.classList.add('active');
    modoVisao = btn.getAttribute('data-view') || 'ambos';

    if (modoVisao === 'energia') {
      blocoEnergia.classList.remove('hidden');
      blocoAgua.classList.add('hidden');
      document.body.classList.add('modo-foco');
    } else if (modoVisao === 'agua') {
      blocoEnergia.classList.add('hidden');
      blocoAgua.classList.remove('hidden');
      document.body.classList.add('modo-foco');
    } else {
      blocoEnergia.classList.remove('hidden');
      blocoAgua.classList.remove('hidden');
      document.body.classList.remove('modo-foco');
    }

    aplicarFiltroTabela();
  });
});

// Eventos dos botões
btnConectarSerial.addEventListener('click', () => void conectarUSBSerial());
btnModoFirebase.addEventListener('click', () => void alternarModoFirebase());
btnSimular.addEventListener('click', () => void alternarSimulacaoEnergia());
btnSimularAgua.addEventListener('click', () => void simularDespejoAgua());

btnLerAgora.addEventListener('click', () => void enviarComandoSerial('p'));
btnZerarESP.addEventListener('click', () => zerarAcumuladoWeb());

btnPausar.addEventListener('click', () => {
  listaPausada = !listaPausada;
  btnPausar.textContent = listaPausada ? '▶ Retomar Lista' : '⏸ Pausar Lista';
});

btnLimpar.addEventListener('click', () => {
  contadorMedidas = 0;
  pillContador.textContent = '0 medidas recebidas';
  tabelaMedidas.innerHTML = `
    <tr class="empty-row">
      <td colspan="7">Lista limpa. Aguardando próxima medida...</td>
    </tr>
  `;
});

btnTelaCheia.addEventListener('click', () => {
  if (!document.fullscreenElement) {
    document.documentElement.requestFullscreen().catch(() => {});
  } else {
    document.exitFullscreen().catch(() => {});
  }
});
