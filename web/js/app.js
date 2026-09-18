const waterValues = [
  145, 152, 139, 160, 147, 150, 143, 156, 149, 151,
  144, 148, 153, 146, 150, 147, 155, 142, 149, 152,
  150, 145, 147, 151, 148, 159, 188, 146, 122, 142
];

const energyValues = [
  4.18, 4.42, 4.11, 4.66, 4.28, 4.47, 4.25, 4.61, 4.38, 4.52,
  4.31, 4.45, 4.58, 4.36, 4.55, 4.41, 4.63, 4.27, 4.49, 4.57,
  4.46, 4.34, 4.40, 4.35, 4.51, 4.62, 5.70, 4.44, 4.56, 4.82
];

const metricConfig = {
  water: {
    label: "Consumo de água",
    unit: "L",
    color: "#18a8c3",
    areaStart: "#22b7d4",
    areaEnd: "#22b7d4",
    values: waterValues,
    comparison: -6.2
  },
  energy: {
    label: "Consumo de energia",
    unit: "kWh",
    color: "#21b978",
    areaStart: "#34d399",
    areaEnd: "#34d399",
    values: energyValues,
    comparison: 5.1
  }
};

let selectedMetric = "water";
let selectedPeriod = 7;
let toastTimer;

function escapeHtml(value) {
  return String(value)
    .replaceAll("&", "&amp;")
    .replaceAll("<", "&lt;")
    .replaceAll(">", "&gt;")
    .replaceAll('"', "&quot;")
    .replaceAll("'", "&#039;");
}

function formatNumber(value, decimals) {
  return new Intl.NumberFormat("pt-BR", {
    minimumFractionDigits: decimals,
    maximumFractionDigits: decimals
  }).format(value);
}

function formatShortDate(date) {
  return new Intl.DateTimeFormat("pt-BR", {
    day: "2-digit",
    month: "2-digit"
  }).format(date);
}

function formatLongDate(date) {
  const text = new Intl.DateTimeFormat("pt-BR", {
    weekday: "short",
    day: "2-digit",
    month: "short"
  }).format(date);

  return text.replaceAll(".", "");
}

function createHistoryData() {
  const today = new Date();

  return waterValues.map(function (water, index) {
    const daysAgo = waterValues.length - 1 - index;
    const date = new Date(today);
    date.setHours(12, 0, 0, 0);
    date.setDate(today.getDate() - daysAgo);

    return {
      date: date,
      label: formatShortDate(date),
      water: water,
      energy: energyValues[index]
    };
  });
}

const historyData = createHistoryData();

function getAverage(values) {
  if (!values.length) {
    return 0;
  }

  return values.reduce(function (total, value) {
    return total + value;
  }, 0) / values.length;
}

function getPointStatus(index) {
  if (index < 1) {
    return { abnormal: false, difference: 0 };
  }

  const previousWater = waterValues.slice(0, index);
  const previousEnergy = energyValues.slice(0, index);
  const averageWater = getAverage(previousWater);
  const averageEnergy = getAverage(previousEnergy);
  const waterDifference = ((waterValues[index] - averageWater) / averageWater) * 100;
  const energyDifference = ((energyValues[index] - averageEnergy) / averageEnergy) * 100;
  const strongestDifference = Math.abs(waterDifference) > Math.abs(energyDifference)
    ? waterDifference
    : energyDifference;

  return {
    abnormal: waterValues[index] > averageWater * 1.2 || energyValues[index] > averageEnergy * 1.2,
    difference: strongestDifference
  };
}

function setDateLabels() {
  const now = new Date();
  const hour = now.getHours();
  let greeting = "Boa noite";

  if (hour >= 5 && hour < 12) {
    greeting = "Bom dia";
  } else if (hour >= 12 && hour < 18) {
    greeting = "Boa tarde";
  }

  document.getElementById("greeting").textContent = greeting + ", Rafael";
  document.getElementById("currentMonth").textContent = new Intl.DateTimeFormat("pt-BR", {
    month: "long"
  }).format(now).replace(/^./, function (letter) {
    return letter.toUpperCase();
  });
  document.getElementById("currentYear").textContent = now.getFullYear();
}

function renderChart() {
  const chart = document.getElementById("consumptionChart");
  const config = metricConfig[selectedMetric];
  const values = config.values.slice(-selectedPeriod);
  const dates = historyData.slice(-selectedPeriod);
  const width = 760;
  const height = 238;
  const padding = { top: 18, right: 20, bottom: 32, left: 42 };
  const innerWidth = width - padding.left - padding.right;
  const innerHeight = height - padding.top - padding.bottom;
  const maxValue = Math.max.apply(null, values) * 1.14;
  const minValue = Math.max(0, Math.min.apply(null, values) * 0.78);
  const range = maxValue - minValue || 1;
  const average = getAverage(values);
  const total = values.reduce(function (sum, value) {
    return sum + value;
  }, 0);
  const labelStep = selectedPeriod === 7 ? 1 : selectedPeriod === 14 ? 2 : 5;

  function xPosition(index) {
    if (values.length === 1) {
      return padding.left + innerWidth / 2;
    }
    return padding.left + (index / (values.length - 1)) * innerWidth;
  }

  function yPosition(value) {
    return padding.top + ((maxValue - value) / range) * innerHeight;
  }

  const points = values.map(function (value, index) {
    return xPosition(index).toFixed(2) + "," + yPosition(value).toFixed(2);
  }).join(" ");

  const areaPoints = padding.left + "," + (padding.top + innerHeight) + " " + points + " " +
    (padding.left + innerWidth) + "," + (padding.top + innerHeight);

  let gridMarkup = "";
  const gridLines = 4;
  for (let index = 0; index <= gridLines; index++) {
    const y = padding.top + (innerHeight / gridLines) * index;
    const value = maxValue - (range / gridLines) * index;
    const decimals = selectedMetric === "energy" ? 1 : 0;

    gridMarkup += `
      <line class="chart-grid-line" x1="${padding.left}" y1="${y}" x2="${width - padding.right}" y2="${y}"></line>
      <text class="chart-axis-label" x="${padding.left - 9}" y="${y + 4}" text-anchor="end">${formatNumber(value, decimals)}</text>
    `;
  }

  let labelMarkup = "";
  dates.forEach(function (item, index) {
    const showLabel = index % labelStep === 0 || index === dates.length - 1;
    if (showLabel) {
      labelMarkup += `<text class="chart-axis-label" x="${xPosition(index)}" y="${height - 8}" text-anchor="middle">${escapeHtml(item.label)}</text>`;
    }
  });

  let pointsMarkup = "";
  values.forEach(function (value, index) {
    const x = xPosition(index);
    const y = yPosition(value);
    const decimals = selectedMetric === "energy" ? 2 : 0;
    const tooltipText = formatNumber(value, decimals) + " " + config.unit;
    const tooltipWidth = selectedMetric === "energy" ? 76 : 64;
    const tooltipX = Math.min(Math.max(x - tooltipWidth / 2, 2), width - tooltipWidth - 2);
    const tooltipY = Math.max(y - 39, 2);

    pointsMarkup += `
      <g class="chart-point" tabindex="0" role="img" aria-label="${escapeHtml(dates[index].label)}: ${escapeHtml(tooltipText)}">
        <circle class="chart-point-ring" cx="${x}" cy="${y}" r="4" style="stroke:${config.color}"></circle>
        <circle class="chart-hit-area" cx="${x}" cy="${y}" r="13"></circle>
        <g class="chart-tooltip" opacity="0">
          <rect x="${tooltipX}" y="${tooltipY}" width="${tooltipWidth}" height="27" rx="7"></rect>
          <text x="${tooltipX + tooltipWidth / 2}" y="${tooltipY + 18}" text-anchor="middle">${escapeHtml(tooltipText)}</text>
        </g>
      </g>
    `;
  });

  chart.innerHTML = `
    <svg viewBox="0 0 ${width} ${height}" role="presentation" aria-hidden="true">
      <defs>
        <linearGradient id="chartAreaGradient" x1="0" y1="0" x2="0" y2="1">
          <stop offset="0%" stop-color="${config.areaStart}" stop-opacity="0.75"></stop>
          <stop offset="100%" stop-color="${config.areaEnd}" stop-opacity="0"></stop>
        </linearGradient>
      </defs>
      ${gridMarkup}
      <polygon class="chart-area" points="${areaPoints}" fill="url(#chartAreaGradient)"></polygon>
      <line class="chart-average" x1="${padding.left}" y1="${yPosition(average)}" x2="${width - padding.right}" y2="${yPosition(average)}"></line>
      <polyline class="chart-line" points="${points}" style="stroke:${config.color}"></polyline>
      ${pointsMarkup}
      ${labelMarkup}
    </svg>
  `;

  const totalDecimals = selectedMetric === "energy" ? 1 : 0;
  document.getElementById("chartTotal").textContent = formatNumber(total, totalDecimals) + " " + config.unit;
  document.getElementById("legendCurrent").textContent = config.label;
  document.getElementById("chartComparison").textContent =
    (config.comparison < 0 ? "−" : "+") +
    formatNumber(Math.abs(config.comparison), 1) +
    "% em relação ao período anterior";
  document.getElementById("chartComparison").style.background = config.comparison < 0 ? "#d9f8e9" : "#fee8e8";
  document.getElementById("chartComparison").style.color = config.comparison < 0 ? "#159661" : "#d94b4b";
  document.querySelector(".legend-dot--primary").style.background = config.color;
  chart.setAttribute(
    "aria-label",
    "Gráfico de " + config.label.toLowerCase() + " nos últimos " + selectedPeriod + " dias"
  );

  chart.querySelectorAll(".chart-point").forEach(function (point) {
    function showTooltip() {
      point.querySelector(".chart-tooltip").setAttribute("opacity", "1");
    }

    function hideTooltip() {
      point.querySelector(".chart-tooltip").setAttribute("opacity", "0");
    }

    point.addEventListener("mouseenter", showTooltip);
    point.addEventListener("mouseleave", hideTooltip);
    point.addEventListener("focus", showTooltip);
    point.addEventListener("blur", hideTooltip);
  });
}

function renderHistoryTable() {
  const tableBody = document.getElementById("historyTableBody");
  const latestEntries = historyData.slice(-7).reverse();

  tableBody.innerHTML = latestEntries.map(function (item, reverseIndex) {
    const originalIndex = historyData.length - 1 - reverseIndex;
    const status = getPointStatus(originalIndex);
    const differenceDirection = status.difference >= 0 ? "up" : "down";
    const arrowPath = differenceDirection === "up"
      ? "m3.7 10.7 3.55-3.55V14h1.5V7.15l3.55 3.55 1.05-1.06L8 4.29 2.65 9.64 3.7 10.7Z"
      : "m3.7 5.3 3.55 3.55V2h1.5v6.85L12.3 5.3l1.05 1.06L8 11.71 2.65 6.36 3.7 5.3Z";
    const dayLabel = reverseIndex === 0 ? "Hoje" : formatLongDate(item.date);

    return `
      <tr>
        <td>
          <span class="table-day">
            <strong>${escapeHtml(dayLabel)}</strong>
            <small>${escapeHtml(item.label)}</small>
          </span>
        </td>
        <td><span class="table-value">${formatNumber(item.water, 0)} L</span></td>
        <td><span class="table-value">${formatNumber(item.energy, 2)} kWh</span></td>
        <td>
          <span class="comparison-value comparison-value--${differenceDirection}">
            <svg aria-hidden="true" viewBox="0 0 16 16"><path d="${arrowPath}"></path></svg>
            ${formatNumber(Math.abs(status.difference), 1)}%
          </span>
        </td>
        <td>
          <span class="table-status ${status.abnormal ? "table-status--warning" : "table-status--normal"}">
            ${status.abnormal ? "Anormal" : "Normal"}
          </span>
        </td>
      </tr>
    `;
  }).join("");
}

function setupMetricControls() {
  document.querySelectorAll("[data-metric]").forEach(function (button) {
    button.addEventListener("click", function () {
      selectedMetric = button.dataset.metric;

      document.querySelectorAll("[data-metric]").forEach(function (item) {
        item.classList.toggle("is-active", item === button);
      });

      renderChart();
    });
  });

  document.getElementById("periodSelect").addEventListener("change", function (event) {
    selectedPeriod = Number(event.target.value);
    renderChart();
  });
}

function setupMobileMenu() {
  const menuButton = document.getElementById("menuButton");
  const overlay = document.getElementById("sidebarOverlay");

  function setMenu(open) {
    document.body.classList.toggle("menu-open", open);
    menuButton.setAttribute("aria-expanded", String(open));
  }

  menuButton.addEventListener("click", function () {
    setMenu(!document.body.classList.contains("menu-open"));
  });

  overlay.addEventListener("click", function () {
    setMenu(false);
  });

  document.querySelectorAll(".nav-link").forEach(function (link) {
    link.addEventListener("click", function () {
      setMenu(false);
    });
  });

  document.addEventListener("keydown", function (event) {
    if (event.key === "Escape") {
      setMenu(false);
    }
  });
}

function setupSectionObserver() {
  const sections = document.querySelectorAll(".section-anchor");
  const links = document.querySelectorAll(".nav-link[data-section]");

  if (!("IntersectionObserver" in window)) {
    return;
  }

  const observer = new IntersectionObserver(function (entries) {
    const visibleEntries = entries.filter(function (entry) {
      return entry.isIntersecting;
    });

    if (!visibleEntries.length) {
      return;
    }

    visibleEntries.sort(function (first, second) {
      return second.intersectionRatio - first.intersectionRatio;
    });

    const activeId = visibleEntries[0].target.id;
    links.forEach(function (link) {
      link.classList.toggle("is-active", link.dataset.section === activeId);
    });
  }, {
    rootMargin: "-20% 0px -58% 0px",
    threshold: [0.05, 0.2, 0.45]
  });

  sections.forEach(function (section) {
    observer.observe(section);
  });
}

function exportCsv() {
  const header = ["Data", "Água (L)", "Energia (kWh)", "Situação"];
  const rows = historyData.map(function (item, index) {
    return [
      item.label,
      formatNumber(item.water, 0),
      formatNumber(item.energy, 2),
      getPointStatus(index).abnormal ? "Anormal" : "Normal"
    ];
  });

  const csv = [header].concat(rows).map(function (row) {
    return row.map(function (cell) {
      return '"' + String(cell).replaceAll('"', '""') + '"';
    }).join(";");
  }).join("\n");

  const blob = new Blob(["\ufeff" + csv], { type: "text/csv;charset=utf-8" });
  const url = URL.createObjectURL(blob);
  const link = document.createElement("a");
  link.href = url;
  link.download = "homewise-historico-demonstracao.csv";
  document.body.appendChild(link);
  link.click();
  link.remove();
  URL.revokeObjectURL(url);
  showToast();
}

function showToast() {
  const toast = document.getElementById("toast");
  window.clearTimeout(toastTimer);
  toast.classList.add("is-visible");
  toastTimer = window.setTimeout(function () {
    toast.classList.remove("is-visible");
  }, 2800);
}

function setupLastUpdate() {
  const lastUpdate = document.getElementById("lastUpdate");
  let minutes = 0;

  window.setInterval(function () {
    minutes += 1;
    lastUpdate.textContent = minutes === 1 ? "há 1 minuto" : "há " + minutes + " minutos";
  }, 60000);
}

function init() {
  setDateLabels();
  renderChart();
  renderHistoryTable();
  setupMetricControls();
  setupMobileMenu();
  setupSectionObserver();
  setupLastUpdate();
  document.getElementById("exportButton").addEventListener("click", exportCsv);
}

document.addEventListener("DOMContentLoaded", init);
