-- ---------------------------------------------------------
-- 1) ASIAKAS
-- ---------------------------------------------------------
CREATE TABLE Asiakas (
  asiakas_id       BIGINT PRIMARY KEY AUTO_INCREMENT,
  etunimi          VARCHAR(100) NOT NULL,
  sukunimi         VARCHAR(100) NOT NULL,
  osoite           VARCHAR(255) NOT NULL,
  puhelinnumero    VARCHAR(30) NOT NULL UNIQUE,
  aktiivinen       BOOLEAN NOT NULL DEFAULT TRUE,
  luotu_aika       DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  paivitetty_aika  DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB;

-- ---------------------------------------------------------
-- 2) TILI (debit jos credit_limit=0, credit jos credit_limit>0)
-- ---------------------------------------------------------
CREATE TABLE Tili (
  tili_id        BIGINT PRIMARY KEY AUTO_INCREMENT,
  tilinumero     VARCHAR(34) NOT NULL UNIQUE,
  omistaja_id    BIGINT NOT NULL,
  saldo_eur      DECIMAL(19,2) NOT NULL DEFAULT 0.00,
  credit_limit   DECIMAL(19,2) NOT NULL DEFAULT 0.00,
  valuutta       CHAR(3) NOT NULL DEFAULT 'EUR',
  tila           ENUM('ACTIVE','LOCKED','CLOSED') NOT NULL DEFAULT 'ACTIVE',
  luotu_aika     DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  CONSTRAINT fk_tili_asiakas
    FOREIGN KEY (omistaja_id) REFERENCES Asiakas(asiakas_id)
    ON DELETE RESTRICT ON UPDATE CASCADE,
  CONSTRAINT chk_saldo_nonneg CHECK (saldo_eur >= 0),
  CONSTRAINT chk_credit_nonneg CHECK (credit_limit >= 0),
  CONSTRAINT chk_valuutta_len CHECK (CHAR_LENGTH(valuutta) = 3)
) ENGINE=InnoDB;

CREATE INDEX idx_tili_omistaja ON Tili(omistaja_id);

-- ---------------------------------------------------------
-- 3) KORTTI
-- ---------------------------------------------------------
CREATE TABLE Kortti (
  kortti_id        BIGINT PRIMARY KEY AUTO_INCREMENT,
  kortti_numero    VARCHAR(32) NOT NULL UNIQUE,
  asiakas_id       BIGINT NOT NULL,
  pin_bcrypt       VARCHAR(100) NOT NULL,
  pin_virhe        INT NOT NULL DEFAULT 0,
  lukossa          DATETIME NULL,
  tila             ENUM('ACTIVE','LOCKED','STOLEN','CLOSED') NOT NULL DEFAULT 'ACTIVE',
  luotu_aika       DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  CONSTRAINT fk_kortti_asiakas
    FOREIGN KEY (asiakas_id) REFERENCES Asiakas(asiakas_id)
    ON DELETE RESTRICT ON UPDATE CASCADE
) ENGINE=InnoDB;

CREATE INDEX idx_kortti_asiakas ON Kortti(asiakas_id);

-- ---------------------------------------------------------
-- 4) KORTTI ↔ TILI (kaksoiskortti roolitettuna)
-- ---------------------------------------------------------
CREATE TABLE KorttiTili (
  kortti_id   BIGINT NOT NULL,
  tili_id     BIGINT NOT NULL,
  rooli       ENUM('DEBIT','CREDIT') NOT NULL DEFAULT 'DEBIT',
  PRIMARY KEY (kortti_id, tili_id),
  CONSTRAINT fk_kt_kortti FOREIGN KEY (kortti_id) REFERENCES Kortti(kortti_id)
    ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT fk_kt_tili FOREIGN KEY (tili_id) REFERENCES Tili(tili_id)
    ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT uq_kortti_rooli UNIQUE (kortti_id, rooli)
) ENGINE=InnoDB;

CREATE INDEX idx_kt_kortti_rooli ON KorttiTili(kortti_id, rooli);
CREATE INDEX idx_kt_tili ON KorttiTili(tili_id);

-- ---------------------------------------------------------
-- 5) TILITAPAHTUMA (tilin poistuessa tapahtumat poistuvat; kortin poistuessa kortti_id -> NULL)
-- ---------------------------------------------------------
CREATE TABLE Tilitapahtuma (
  tapahtuma_id       BIGINT PRIMARY KEY AUTO_INCREMENT,
  tili_id            BIGINT NOT NULL,
  kortti_id          BIGINT NULL,
  laji               ENUM('DEPOSIT','WITHDRAWAL') NOT NULL,
  summa_eur          DECIMAL(19,2) NOT NULL,
  valuutta           CHAR(3) NOT NULL DEFAULT 'EUR',
  tapahtuma_aika     DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  saldo_hetkellinen  DECIMAL(19,2) NULL,
  CONSTRAINT fk_tt_tili FOREIGN KEY (tili_id) REFERENCES Tili(tili_id)
    ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT fk_tt_kortti FOREIGN KEY (kortti_id) REFERENCES Kortti(kortti_id)
    ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT chk_summa_pos CHECK (summa_eur > 0),
  CONSTRAINT chk_tt_valuutta_len CHECK (CHAR_LENGTH(valuutta) = 3)
) ENGINE=InnoDB;

CREATE INDEX idx_tili_aika ON Tilitapahtuma(tili_id, tapahtuma_aika);
CREATE INDEX idx_kortti_aika ON Tilitapahtuma(kortti_id, tapahtuma_aika);

-- ---------------------------------------------------------
-- 6) PIN-POOLI (pooli-3) — yksi rivi per kortti
-- ---------------------------------------------------------
CREATE TABLE PinPool (
  kortti_id        BIGINT PRIMARY KEY,
  virhelaskuri     TINYINT UNSIGNED NOT NULL DEFAULT 0,
  viimeisin_yritys DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
  lukossa_asti     DATETIME NULL,
  CONSTRAINT fk_pinpool_kortti FOREIGN KEY (kortti_id) REFERENCES Kortti(kortti_id)
    ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT chk_virhelaskuri_range CHECK (virhelaskuri BETWEEN 0 AND 3)
) ENGINE=InnoDB;

-- ---------------------------------------------------------
-- 7) Päivittäinen nostoraja (TILIKOHTAINEN)
-- ---------------------------------------------------------
CREATE TABLE PaivittainenRaja (
  tili_id        BIGINT NOT NULL,
  paiva          DATE NOT NULL,
  nostettu_eur   DECIMAL(19,2) NOT NULL DEFAULT 0.00,
  PRIMARY KEY (tili_id, paiva),
  CONSTRAINT fk_pr_tili FOREIGN KEY (tili_id) REFERENCES Tili(tili_id)
    ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT chk_pr_nonneg CHECK (nostettu_eur >= 0)
) ENGINE=InnoDB;

CREATE INDEX idx_pr_paiva ON PaivittainenRaja(paiva);

-- ---------------------------------------------------------
-- SIEMENDATA
-- ---------------------------------------------------------

INSERT INTO Asiakas(etunimi, sukunimi, osoite, puhelinnumero)
VALUES ('Test', 'Asiakas', 'Katu 1, 90100 Oulu', '+358401234567');

INSERT INTO Tili(tilinumero, omistaja_id, saldo_eur, credit_limit, valuutta)
VALUES
('FI00 1234 5600 0000 01', 1, 500.00,   0.00, 'EUR'),  -- DEBIT id=1
('FI00 1234 5600 0000 02', 1,   0.00, 2000.00, 'EUR'); -- CREDIT id=2

-- Vaihda alla oleva bcrypt-hash oikeaksi (PIN "1234" esimerkkinä)
INSERT INTO Kortti(kortti_numero, asiakas_id, pin_bcrypt)
VALUES ('5555-6666-7777-8888', 1, '$2a$10$5AJUFbHxlcwQ.cH/YoU2gOUiM1MfhhW7FRj88sLAXnThd3VYKH9B.');

INSERT INTO KorttiTili(kortti_id, tili_id, rooli)
VALUES
(1, 1, 'DEBIT'),
(1, 2, 'CREDIT');

INSERT INTO PinPool(kortti_id, virhelaskuri, viimeisin_yritys, lukossa_asti)
VALUES (1, 0, CURRENT_TIMESTAMP, NULL);

INSERT INTO PaivittainenRaja(tili_id, paiva, nostettu_eur)
VALUES (1, CURRENT_DATE(), 0.00);

INSERT INTO Tilitapahtuma(tili_id, kortti_id, laji, summa_eur, valuutta, saldo_hetkellinen)
VALUES
(1, 1, 'DEPOSIT',    100.00, 'EUR', 600.00),
(1, 1, 'WITHDRAWAL',  50.00, 'EUR', 550.00);
