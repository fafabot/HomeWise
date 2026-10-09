require("dotenv").config();

const express = require("express");
const cors = require("cors");
const mysql = require("mysql2/promise");

const app = express();
const PORT = Number(process.env.PORT) || 3000;
const DEFAULT_DEVICE_ID = process.env.DEVICE_ID || "central_homewise_01";

const pool = mysql.createPool({
    host: process.env.DB_HOST || "127.0.0.1",
    port: Number(process.env.DB_PORT) || 3306,
    user: process.env.DB_USER || "root",
    password: process.env.DB_PASSWORD || "",
    database: process.env.DB_NAME || "homewise",
    waitForConnections: true,
    connectionLimit: 10,
    queueLimit: 0,
    decimalNumbers: true
});

app.use(cors({ origin: process.env.CORS_ORIGIN || "*" }));
app.use(express.json({ limit: "32kb" }));

function numberOrUndefined(value) {
    if (value === undefined || value === null || value === "") return undefined;
    const number = Number(value);
    return Number.isFinite(number) ? number : undefined;
}

function firstDefined(...values) {
    return values.find((value) => value !== undefined && value !== null);
}

function normalizeReading(body) {
    const agua = numberOrUndefined(firstDefined(body.consumo_agua_litros, body.agua));
    const vazao = numberOrUndefined(firstDefined(body.vazao_l_min, body.vazao, 0));
    const energia = numberOrUndefined(firstDefined(body.energia_kwh, body.energia));
    const potencia = numberOrUndefined(firstDefined(body.potencia_w, body.potencia));
    const tensao = numberOrUndefined(firstDefined(body.tensao_v, body.tensao, 0));

    const values = { agua, vazao, energia, potencia, tensao };
    const invalidFields = Object.entries(values)
        .filter(([field, value]) =>
            value === undefined || value < 0 ||
            (field === "tensao" && value > 300) ||
            (field === "vazao" && value > 10000) ||
            (field === "potencia" && value > 100000) ||
            (field === "energia" && value > 100000000) ||
            (field === "agua" && value > 100000000)
        )
        .map(([field]) => field);

    if (invalidFields.length) return { error: invalidFields };

    const deviceId = firstDefined(body.dispositivo_id, DEFAULT_DEVICE_ID);
    if (typeof deviceId !== "string" || !deviceId.trim() || deviceId.length > 50) {
        return { error: ["dispositivo_id"] };
    }

    return {
        data: {
            dispositivo_id: deviceId.trim(),
            consumo_agua_litros: agua,
            vazao_l_min: vazao,
            potencia_w: potencia,
            energia_kwh: energia,
            tensao_v: tensao
        }
    };
}

app.get("/api/health", async (req, res) => {
    try {
        await pool.query("SELECT 1");
        return res.status(200).json({
            sucesso: true,
            api: "online",
            banco: "conectado",
            mensagem: "API HomeWise e MySQL estao funcionando."
        });
    } catch (error) {
        console.error("[API] MySQL indisponivel:", error.message);
        return res.status(503).json({
            sucesso: false,
            api: "online",
            banco: "indisponivel",
            mensagem: "A API iniciou, mas nao conseguiu conectar ao MySQL. Confira as variaveis DB_* e o banco."
        });
    }
});

app.post("/api/dados", async (req, res) => {
    if (!req.body || typeof req.body !== "object" || Array.isArray(req.body)) {
        return res.status(400).json({
            sucesso: false,
            mensagem: "Envie um objeto JSON com os dados da leitura."
        });
    }

    const normalized = normalizeReading(req.body);
    if (normalized.error) {
        return res.status(400).json({
            sucesso: false,
            mensagem: "Dados ausentes ou invalidos.",
            camposInvalidos: normalized.error
        });
    }

    const reading = normalized.data;
    try {
        const [result] = await pool.execute(
            `INSERT INTO leituras
                (dispositivo_id, consumo_agua_litros, vazao_l_min, potencia_w, energia_kwh, tensao_v)
             VALUES (?, ?, ?, ?, ?, ?)`,
            [
                reading.dispositivo_id, reading.consumo_agua_litros, reading.vazao_l_min,
                reading.potencia_w, reading.energia_kwh, reading.tensao_v
            ]
        );

        return res.status(201).json({
            sucesso: true,
            mensagem: "Leitura salva no MySQL.",
            id: result.insertId,
            dados: reading
        });
    } catch (error) {
        console.error("[API] Erro ao salvar leitura:", error.message);
        return res.status(503).json({
            sucesso: false,
            mensagem: "Nao foi possivel salvar a leitura. Confira a conexao e a tabela leituras."
        });
    }
});

app.get("/api/dashboard", async (req, res) => {
    const deviceId = typeof req.query.dispositivo_id === "string"
        ? req.query.dispositivo_id
        : DEFAULT_DEVICE_ID;

    try {
        const [latestRows] = await pool.execute(
            `SELECT id, dispositivo_id, consumo_agua_litros, vazao_l_min,
                    potencia_w, energia_kwh, tensao_v, criado_em
             FROM leituras WHERE dispositivo_id = ?
             ORDER BY criado_em DESC, id DESC LIMIT 1`,
            [deviceId]
        );
        const [historyRows] = await pool.execute(
            `SELECT id, dispositivo_id, consumo_agua_litros, vazao_l_min,
                    potencia_w, energia_kwh, tensao_v, criado_em
             FROM leituras WHERE dispositivo_id = ?
             ORDER BY criado_em DESC, id DESC LIMIT 30`,
            [deviceId]
        );

        return res.status(200).json({
            sucesso: true,
            dispositivo_id: deviceId,
            ultima_leitura: latestRows[0] || null,
            historico_recente: historyRows.reverse()
        });
    } catch (error) {
        console.error("[API] Erro ao consultar dashboard:", error.message);
        return res.status(503).json({
            sucesso: false,
            mensagem: "Nao foi possivel consultar os dados no MySQL."
        });
    }
});

app.get("/api/historico", async (req, res) => {
    const deviceId = typeof req.query.dispositivo_id === "string"
        ? req.query.dispositivo_id
        : DEFAULT_DEVICE_ID;
    const requestedLimit = Number.parseInt(req.query.limite, 10);
    const limit = Number.isInteger(requestedLimit) ? Math.min(Math.max(requestedLimit, 1), 500) : 100;

    try {
        const [rows] = await pool.execute(
            `SELECT id, dispositivo_id, consumo_agua_litros, vazao_l_min,
                    potencia_w, energia_kwh, tensao_v, criado_em
             FROM leituras WHERE dispositivo_id = ?
             ORDER BY criado_em DESC, id DESC LIMIT ?`,
            [deviceId, limit]
        );
        return res.status(200).json({
            sucesso: true,
            dispositivo_id: deviceId,
            quantidade: rows.length,
            dados: rows
        });
    } catch (error) {
        console.error("[API] Erro ao consultar historico:", error.message);
        return res.status(503).json({
            sucesso: false,
            mensagem: "Nao foi possivel consultar o historico no MySQL."
        });
    }
});

app.use((err, req, res, next) => {
    if (err instanceof SyntaxError && err.status === 400 && "body" in err) {
        return res.status(400).json({ sucesso: false, mensagem: "JSON invalido." });
    }
    console.error("[API] Erro interno:", err.message);
    return res.status(500).json({ sucesso: false, mensagem: "Erro interno da API." });
});

const server = app.listen(PORT, "0.0.0.0", () => {
    console.log(`[API] HomeWise rodando na porta ${PORT}`);
    console.log(`[API] Banco configurado: ${process.env.DB_NAME || "homewise"}`);
});

async function shutdown() {
    server.close(async () => {
        await pool.end();
        process.exit(0);
    });
}

process.on("SIGINT", shutdown);
process.on("SIGTERM", shutdown);
