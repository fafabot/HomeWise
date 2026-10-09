require("dotenv").config();

const express = require("express");
const cors = require("cors");
const path = require("path");
const fs = require("fs");
const { initializeApp, cert } = require("firebase-admin/app");
const { getFirestore, FieldValue } = require("firebase-admin/firestore");

const app = express();
const PORT = Number(process.env.PORT) || 3000;
const DEFAULT_DEVICE_ID = process.env.DEVICE_ID || "central_homewise_01";

// Inicializacao do Firebase Admin SDK
let db = null;

try {
    if (process.env.FIREBASE_SERVICE_ACCOUNT_KEY) {
        const serviceAccount = JSON.parse(process.env.FIREBASE_SERVICE_ACCOUNT_KEY);
        const firebaseApp = initializeApp({
            credential: cert(serviceAccount)
        });
        db = getFirestore(firebaseApp);
        console.log("[API] Firebase conectado via FIREBASE_SERVICE_ACCOUNT_KEY");
    } else {
        const localKeyPath = path.join(__dirname, "serviceAccountKey.json");

        if (fs.existsSync(localKeyPath)) {
            const serviceAccount = require(localKeyPath);
            const firebaseApp = initializeApp({
                credential: cert(serviceAccount)
            });
            db = getFirestore(firebaseApp);
            console.log("[API] Firebase conectado com sucesso via serviceAccountKey.json local");
        } else {
            const firebaseApp = initializeApp();
            db = getFirestore(firebaseApp);
            console.log("[API] Firebase inicializado com Application Default Credentials");
        }
    }
} catch (error) {
    console.error("[API] Erro ao inicializar Firebase Admin:", error.message);
}

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

// Health check
app.get("/api/health", async (req, res) => {
    if (!db) {
        return res.status(503).json({
            sucesso: false,
            api: "online",
            banco: "desconectado",
            mensagem: "API online, mas o Firebase nao foi inicializado. Verifique serviceAccountKey.json."
        });
    }

    return res.status(200).json({
        sucesso: true,
        api: "online",
        banco: "conectado",
        mensagem: "API HomeWise e Firebase Firestore estao funcionando perfeitamente."
    });
});

// Ingestao de dados da Central (ESP8266)
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

    if (!db) {
        return res.status(503).json({
            sucesso: false,
            mensagem: "Firebase Firestore nao conectado no servidor."
        });
    }

    const reading = normalized.data;
    try {
        const docData = {
            ...reading,
            criado_em: FieldValue.serverTimestamp()
        };

        const docRef = await db.collection("leituras").add(docData);

        return res.status(201).json({
            sucesso: true,
            mensagem: "Leitura salva no Firebase Firestore.",
            id: docRef.id,
            dados: reading
        });
    } catch (error) {
        console.error("[API] Erro ao salvar leitura no Firestore:", error.message);
        return res.status(503).json({
            sucesso: false,
            mensagem: "Nao foi possivel salvar a leitura no Firebase Firestore."
        });
    }
});

// Dashboard: Ultima leitura e historico recente
app.get("/api/dashboard", async (req, res) => {
    if (!db) {
        return res.status(503).json({ sucesso: false, mensagem: "Firebase nao conectado." });
    }

    const deviceId = typeof req.query.dispositivo_id === "string"
        ? req.query.dispositivo_id
        : DEFAULT_DEVICE_ID;

    try {
        const snapshot = await db.collection("leituras")
            .where("dispositivo_id", "==", deviceId)
            .orderBy("criado_em", "desc")
            .limit(30)
            .get();

        const docs = snapshot.docs.map(doc => {
            const data = doc.data();
            return {
                id: doc.id,
                ...data,
                criado_em: data.criado_em ? data.criado_em.toDate().toISOString() : new Date().toISOString()
            };
        });

        const ultima_leitura = docs.length > 0 ? docs[0] : null;
        const historico_recente = [...docs].reverse();

        return res.status(200).json({
            sucesso: true,
            dispositivo_id: deviceId,
            ultima_leitura,
            historico_recente
        });
    } catch (error) {
        console.error("[API] Erro ao consultar dashboard no Firestore:", error.message);
        return res.status(503).json({
            sucesso: false,
            mensagem: "Nao foi possivel consultar os dados no Firebase."
        });
    }
});

// Historico completo
app.get("/api/historico", async (req, res) => {
    if (!db) {
        return res.status(503).json({ sucesso: false, mensagem: "Firebase nao conectado." });
    }

    const deviceId = typeof req.query.dispositivo_id === "string"
        ? req.query.dispositivo_id
        : DEFAULT_DEVICE_ID;

    const requestedLimit = Number.parseInt(req.query.limite, 10);
    const limit = Number.isInteger(requestedLimit) ? Math.min(Math.max(requestedLimit, 1), 500) : 100;

    try {
        const snapshot = await db.collection("leituras")
            .where("dispositivo_id", "==", deviceId)
            .orderBy("criado_em", "desc")
            .limit(limit)
            .get();

        const docs = snapshot.docs.map(doc => {
            const data = doc.data();
            return {
                id: doc.id,
                ...data,
                criado_em: data.criado_em ? data.criado_em.toDate().toISOString() : new Date().toISOString()
            };
        });

        return res.status(200).json({
            sucesso: true,
            dispositivo_id: deviceId,
            quantidade: docs.length,
            dados: docs
        });
    } catch (error) {
        console.error("[API] Erro ao consultar historico no Firestore:", error.message);
        return res.status(503).json({
            sucesso: false,
            mensagem: "Nao foi possivel consultar o historico no Firebase."
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
    console.log(`[API] Armazenamento: Firebase Cloud Firestore`);
});

async function shutdown() {
    server.close(() => {
        process.exit(0);
    });
}

process.on("SIGINT", shutdown);
process.on("SIGTERM", shutdown);
