CREATE DATABASE IF NOT EXISTS homewise
  CHARACTER SET utf8mb4
  COLLATE utf8mb4_unicode_ci;

USE homewise;

CREATE TABLE IF NOT EXISTS leituras (
    id BIGINT UNSIGNED NOT NULL AUTO_INCREMENT,
    dispositivo_id VARCHAR(50) NOT NULL,
    consumo_agua_litros DECIMAL(12, 3) NOT NULL DEFAULT 0.000,
    vazao_l_min DECIMAL(10, 3) NOT NULL DEFAULT 0.000,
    potencia_w DECIMAL(12, 3) NOT NULL DEFAULT 0.000,
    energia_kwh DECIMAL(14, 6) NOT NULL DEFAULT 0.000000,
    tensao_v DECIMAL(8, 2) NOT NULL DEFAULT 0.00,
    criado_em TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (id),
    INDEX idx_leituras_dispositivo_data (dispositivo_id, criado_em),
    INDEX idx_leituras_criado_em (criado_em)
) ENGINE=InnoDB;
