const express = require("express");

const app = express();

const PORT = process.env.PORT || 3000;

app.use(express.json());

app.get("/api/health", (req, res) => {
    res.status(200).json({
        sucesso: true,
        mensagem: "API HomeWise online"
    });
});

app.post("/api/dados", (req, res) => {
    const { agua, vazao, energia, potencia } = req.body;

    const valores = { agua, vazao, energia, potencia };

    const camposInvalidos = Object.entries(valores)
        .filter(([_, valor]) => typeof valor !== "number" || !Number.isFinite(valor) || valor < 0)
        .map(([campo]) => campo);

    if (camposInvalidos.length > 0) {
        return res.status(400).json({
            sucesso: false,
            mensagem: "Dados invalidos.",
            camposInvalidos
        });
    }

    console.log("[API] Dados recebidos:", valores);

    return res.status(200).json({
        sucesso: true,
        mensagem: "Dados recebidos com sucesso",
        dados: valores
    });
});

app.use((err, req, res, next) => {
    if (err instanceof SyntaxError && err.status === 400 && "body" in err) {
        return res.status(400).json({
            sucesso: false,
            mensagem: "JSON invalido."
        });
    }

    console.error("[API] Erro interno:", err);

    return res.status(500).json({
        sucesso: false,
        mensagem: "Erro interno da API."
    });
});

app.listen(PORT, () => {
    console.log(`[API] HomeWise rodando na porta ${PORT}`);
});
