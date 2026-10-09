# API HomeWise

API Node.js/Express que recebe leituras do ESP8266 e salva os registros no MySQL.

## 1. Preparar o banco

1. Abra o MySQL Workbench ou o cliente MySQL.
2. Execute o arquivo sql/schema.sql.
3. O script cria o banco homewise e a tabela leituras.

Se voce ja criou a tabela leituras com uma versao anterior do esquema, adicione a coluna de vazao antes de iniciar a API:

```sql
USE homewise;
ALTER TABLE leituras
  ADD COLUMN vazao_l_min DECIMAL(10, 3) NOT NULL DEFAULT 0.000
  AFTER consumo_agua_litros;
```

Execute esse ALTER somente se a coluna vazao_l_min ainda nao existir.

Crie um usuario dedicado para a API, usando uma senha sua no lugar do texto de exemplo:

```sql
CREATE USER 'homewise_app'@'127.0.0.1' IDENTIFIED BY 'SUA_SENHA_FORTE_AQUI';
GRANT SELECT, INSERT ON homewise.leituras TO 'homewise_app'@'127.0.0.1';
FLUSH PRIVILEGES;
```

Se esse usuario ja existir, use ALTER USER para trocar a senha em vez de executar CREATE USER novamente.

## 2. Configurar a API

Na pasta api, copie .env.example para .env e preencha DB_PASSWORD com a mesma senha definida para homewise_app. O arquivo .env e ignorado pelo Git e nao deve ser publicado.

Instale as dependencias e inicie:

```bash
npm install
npm run check
npm start
```

A API escuta na porta definida por PORT (3000 por padrao) e em todas as interfaces de rede, para permitir conexoes do ESP8266 na mesma LAN.

## 3. Rotas

- GET /api/health — informa se a API e o MySQL estao acessiveis.
- POST /api/dados — valida e grava uma leitura.
- GET /api/dashboard?dispositivo_id=central_homewise_01 — devolve a ultima leitura e ate 30 leituras recentes.
- GET /api/historico?dispositivo_id=central_homewise_01&limite=100 — consulta ate 500 registros.

Exemplo de corpo para POST /api/dados:

```json
{
  "dispositivo_id": "central_homewise_01",
  "consumo_agua_litros": 12.5,
  "vazao_l_min": 1.7,
  "potencia_w": 350,
  "energia_kwh": 0.82,
  "tensao_v": 127.4
}
```

A API tambem aceita os nomes curtos agua, vazao, energia e potencia para facilitar testes antigos.

## 4. Conectar o ESP8266

No arquivo include/config.h, altere HOMEWISE_API_URL para o IP local do computador que executa a API, por exemplo http://192.168.1.25:3000/api/dados.

No arquivo local ignorado pelo GitHub, include/credenciais.h, informe o nome e a senha da rede Wi-Fi. O modelo esta em include/credenciais.h.example. Use uma rede de 2,4 GHz compativel com o ESP8266.

O ESP8266 e o computador precisam estar na mesma rede, e o firewall do computador deve permitir conexoes de entrada na porta 3000. Nao use localhost no ESP8266.

## Observacao sobre o historico

Cada POST grava uma leitura instantanea. Os valores de agua e energia enviados pelo firmware sao acumulados desde o ultimo reset do consumo no proprio dispositivo; portanto, nao some todas as linhas como se fossem consumos independentes. A agregacao por dia deve ser implementada depois que a regra de virada do dia estiver usando data/hora reais.
