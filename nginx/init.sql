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

-- -------------------------
-- Asiakkaat
-- -------------------------
INSERT INTO Asiakas(etunimi, sukunimi, osoite, puhelinnumero) VALUES
('Aku',   'Ankka',  'Ankkalinna, Rämeenkatu 1',      '+358404200001'),
('Roope', 'Ankka',  'Ankkalinna, Rahalaiturintie 1', '+358404200002'),
('Iines', 'Ankka',  'Ankkalinna, Joutsentie 3',      '+358404200003'),
('Tupu',  'Ankka',  'Ankkalinna, Kolmostie 10',      '+358404200004'),
('Hupu',  'Ankka',  'Ankkalinna, Kolmostie 10',      '+358404200005'),
('Lupu',  'Ankka',  'Ankkalinna, Kolmostie 10',      '+358404200006'),
('Pelle', 'Peloton','Ankkalinna, Keksijänkuja 2',    '+358404200007'),
('Hannu', 'Hanhi',  'Ankkalinna, Onnentie 7',        '+358404200008');

-- -------------------------
-- Tilit (DEBIT ja CREDIT)
-- -------------------------
-- Aku: debit + credit
INSERT INTO Tili(tilinumero, omistaja_id, saldo_eur, credit_limit, valuutta)
SELECT 'FI00 5555 1111 0000 01', a.asiakas_id, 350.00,   0.00, 'EUR'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200001';
INSERT INTO Tili(tilinumero, omistaja_id, saldo_eur, credit_limit, valuutta)
SELECT 'FI00 5555 1111 0000 02', a.asiakas_id,   0.00, 1500.00, 'EUR'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200001';

-- Roope: debit (isompi saldo)
INSERT INTO Tili(tilinumero, omistaja_id, saldo_eur, credit_limit, valuutta)
SELECT 'FI00 7777 2222 0000 01', a.asiakas_id, 1000000.00, 0.00, 'EUR'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200002';

-- Iines: debit
INSERT INTO Tili(tilinumero, omistaja_id, saldo_eur, credit_limit, valuutta)
SELECT 'FI00 6666 3333 0000 01', a.asiakas_id, 800.00, 0.00, 'EUR'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200003';

-- Pelle: debit
INSERT INTO Tili(tilinumero, omistaja_id, saldo_eur, credit_limit, valuutta)
SELECT 'FI00 8888 4444 0000 01', a.asiakas_id, 120.00, 0.00, 'EUR'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200007';

-- Hannu: debit + credit
INSERT INTO Tili(tilinumero, omistaja_id, saldo_eur, credit_limit, valuutta)
SELECT 'FI00 9999 5555 0000 01', a.asiakas_id, 60.00,  0.00, 'EUR'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200008';
INSERT INTO Tili(tilinumero, omistaja_id, saldo_eur, credit_limit, valuutta)
SELECT 'FI00 9999 5555 0000 02', a.asiakas_id,  0.00,  5000.00, 'EUR'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200008';

-- -------------------------
-- Kortit (yksi kortti / henkilö; Aku & Hannu linkitetään debit+creditiin)
-- HUOM: korvaa bcrypt-hashit oikeilla arvoilla!
-- -------------------------
-- Aku (PIN 1234 → korvaa oikealla bcryptillä)
INSERT INTO Kortti(kortti_numero, asiakas_id, pin_bcrypt)
SELECT '4000-1234-5678-0001', a.asiakas_id, '$2a$10$wci/dfpCNirwuoWybry.oOEltlwdZic/eSpyBX73HbTHugTMs8f12'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200001';

-- Roope (PIN 1111)
INSERT INTO Kortti(kortti_numero, asiakas_id, pin_bcrypt)
SELECT '4000-1111-1111-0001', a.asiakas_id, '$2a$10$VdcXNG3ejDiNVJLgjQh6culS5s3Z.EV50fjDpNj2difK37uwjDa3O'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200002';

-- Iines (PIN 2468)
INSERT INTO Kortti(kortti_numero, asiakas_id, pin_bcrypt)
SELECT '4000-2468-1357-0001', a.asiakas_id, '$2a$10$epiFrZba.dIdq0GzQyFXB./95T7mbyz1xBHVpQgqs31.hR9KvaRXi'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200003';

-- Pelle (PIN 0000)
INSERT INTO Kortti(kortti_numero, asiakas_id, pin_bcrypt)
SELECT '4000-0000-0000-0001', a.asiakas_id, '$2a$10$0xyVmm1TKx8RJLkdhiQ9QOJSKeU5cRPH8TX2FxUsc/CUtpYyvQdd2'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200007';

-- Hannu (PIN 7777)
INSERT INTO Kortti(kortti_numero, asiakas_id, pin_bcrypt)
SELECT '4000-7777-5555-0001', a.asiakas_id, '$2a$10$o9ljtVitmV8N5LH57U6zbuvnJLN4JgaLE37foKxxc/2jbrjt8H2L2'
FROM Asiakas a WHERE a.puhelinnumero = '+358404200008';

-- -------------------------
-- KorttiTili (DEBIT/CREDIT -roolit)
-- -------------------------
-- Aku: linkitä molempiin tileihin
INSERT INTO KorttiTili(kortti_id, tili_id, rooli)
SELECT k.kortti_id, t.tili_id, 'DEBIT'
FROM Kortti k
JOIN Asiakas a ON a.asiakas_id = k.asiakas_id
JOIN Tili t ON t.omistaja_id = a.asiakas_id AND t.tilinumero = 'FI00 5555 1111 0000 01'
WHERE k.kortti_numero = '4000-1234-5678-0001';

INSERT INTO KorttiTili(kortti_id, tili_id, rooli)
SELECT k.kortti_id, t.tili_id, 'CREDIT'
FROM Kortti k
JOIN Asiakas a ON a.asiakas_id = k.asiakas_id
JOIN Tili t ON t.omistaja_id = a.asiakas_id AND t.tilinumero = 'FI00 5555 1111 0000 02'
WHERE k.kortti_numero = '4000-1234-5678-0001';

-- Roope: vain debit-tili
INSERT INTO KorttiTili(kortti_id, tili_id, rooli)
SELECT k.kortti_id, t.tili_id, 'DEBIT'
FROM Kortti k
JOIN Asiakas a ON a.asiakas_id = k.asiakas_id
JOIN Tili t ON t.omistaja_id = a.asiakas_id AND t.tilinumero = 'FI00 7777 2222 0000 01'
WHERE k.kortti_numero = '4000-1111-1111-0001';

-- Iines: vain debit-tili
INSERT INTO KorttiTili(kortti_id, tili_id, rooli)
SELECT k.kortti_id, t.tili_id, 'DEBIT'
FROM Kortti k
JOIN Asiakas a ON a.asiakas_id = k.asiakas_id
JOIN Tili t ON t.omistaja_id = a.asiakas_id AND t.tilinumero = 'FI00 6666 3333 0000 01'
WHERE k.kortti_numero = '4000-2468-1357-0001';

-- Pelle: vain debit-tili
INSERT INTO KorttiTili(kortti_id, tili_id, rooli)
SELECT k.kortti_id, t.tili_id, 'DEBIT'
FROM Kortti k
JOIN Asiakas a ON a.asiakas_id = k.asiakas_id
JOIN Tili t ON t.omistaja_id = a.asiakas_id AND t.tilinumero = 'FI00 8888 4444 0000 01'
WHERE k.kortti_numero = '4000-0000-0000-0001';

-- Hannu: debit + credit
INSERT INTO KorttiTili(kortti_id, tili_id, rooli)
SELECT k.kortti_id, t.tili_id, 'DEBIT'
FROM Kortti k
JOIN Asiakas a ON a.asiakas_id = k.asiakas_id
JOIN Tili t ON t.omistaja_id = a.asiakas_id AND t.tilinumero = 'FI00 9999 5555 0000 01'
WHERE k.kortti_numero = '4000-7777-5555-0001';

INSERT INTO KorttiTili(kortti_id, tili_id, rooli)
SELECT k.kortti_id, t.tili_id, 'CREDIT'
FROM Kortti k
JOIN Asiakas a ON a.asiakas_id = k.asiakas_id
JOIN Tili t ON t.omistaja_id = a.asiakas_id AND t.tilinumero = 'FI00 9999 5555 0000 02'
WHERE k.kortti_numero = '4000-7777-5555-0001';

-- -------------------------
-- PIN-poolit (alustetaan 0-yritystä)
-- -------------------------
INSERT INTO PinPool(kortti_id, virhelaskuri)
SELECT k.kortti_id, 0
FROM Kortti k
WHERE k.kortti_numero IN (
  '4000-1234-5678-0001','4000-1111-1111-0001','4000-2468-1357-0001',
  '4000-0000-0000-0001','4000-7777-5555-0001'
)
AND NOT EXISTS (SELECT 1 FROM PinPool p WHERE p.kortti_id = k.kortti_id);

-- -------------------------
-- Päivittäiset rajat (nollasta alkaen tälle päivälle)
-- -------------------------
-- Aku debit + credit
INSERT INTO PaivittainenRaja(tili_id, paiva, nostettu_eur)
SELECT t.tili_id, CURRENT_DATE(), 0.00
FROM Tili t WHERE t.tilinumero IN ('FI00 5555 1111 0000 01','FI00 5555 1111 0000 02')
ON DUPLICATE KEY UPDATE nostettu_eur = VALUES(nostettu_eur);

-- Roope debit
INSERT INTO PaivittainenRaja(tili_id, paiva, nostettu_eur)
SELECT t.tili_id, CURRENT_DATE(), 0.00
FROM Tili t WHERE t.tilinumero = 'FI00 7777 2222 0000 01'
ON DUPLICATE KEY UPDATE nostettu_eur = VALUES(nostettu_eur);

-- Iines debit
INSERT INTO PaivittainenRaja(tili_id, paiva, nostettu_eur)
SELECT t.tili_id, CURRENT_DATE(), 0.00
FROM Tili t WHERE t.tilinumero = 'FI00 6666 3333 0000 01'
ON DUPLICATE KEY UPDATE nostettu_eur = VALUES(nostettu_eur);

-- Pelle debit
INSERT INTO PaivittainenRaja(tili_id, paiva, nostettu_eur)
SELECT t.tili_id, CURRENT_DATE(), 0.00
FROM Tili t WHERE t.tilinumero = 'FI00 8888 4444 0000 01'
ON DUPLICATE KEY UPDATE nostettu_eur = VALUES(nostettu_eur);

-- Hannu debit + credit
INSERT INTO PaivittainenRaja(tili_id, paiva, nostettu_eur)
SELECT t.tili_id, CURRENT_DATE(), 0.00
FROM Tili t WHERE t.tilinumero IN ('FI00 9999 5555 0000 01','FI00 9999 5555 0000 02')
ON DUPLICATE KEY UPDATE nostettu_eur = VALUES(nostettu_eur);

-- -------------------------
-- Tapahtumat (muutama rivi per tilanne)
-- -------------------------
-- Aku: 100€ nosto, 50€ talletus debit-tililtä
INSERT INTO Tilitapahtuma(tili_id, kortti_id, laji, summa_eur, valuutta, saldo_hetkellinen)
SELECT t.tili_id, k.kortti_id, 'WITHDRAWAL', 100.00, 'EUR', t.saldo_eur - 100.00
FROM Tili t
JOIN Asiakas a ON a.asiakas_id = t.omistaja_id
JOIN Kortti k ON k.asiakas_id = a.asiakas_id
WHERE t.tilinumero = 'FI00 5555 1111 0000 01' AND k.kortti_numero = '4000-1234-5678-0001';

INSERT INTO Tilitapahtuma(tili_id, kortti_id, laji, summa_eur, valuutta, saldo_hetkellinen)
SELECT t.tili_id, k.kortti_id, 'DEPOSIT', 50.00, 'EUR', t.saldo_eur - 50.00 + 150.00  -- saldo_hetkellinen on esimerkinomainen
FROM Tili t
JOIN Asiakas a ON a.asiakas_id = t.omistaja_id
JOIN Kortti k ON k.asiakas_id = a.asiakas_id
WHERE t.tilinumero = 'FI00 5555 1111 0000 01' AND k.kortti_numero = '4000-1234-5678-0001';

-- Roope: 1000€ talletus
INSERT INTO Tilitapahtuma(tili_id, kortti_id, laji, summa_eur, valuutta, saldo_hetkellinen)
SELECT t.tili_id, k.kortti_id, 'DEPOSIT', 1000.00, 'EUR', t.saldo_eur + 1000.00
FROM Tili t
JOIN Asiakas a ON a.asiakas_id = t.omistaja_id
JOIN Kortti k ON k.asiakas_id = a.asiakas_id
WHERE t.tilinumero = 'FI00 7777 2222 0000 01' AND k.kortti_numero = '4000-1111-1111-0001';

-- Iines: 40€ nosto
INSERT INTO Tilitapahtuma(tili_id, kortti_id, laji, summa_eur, valuutta, saldo_hetkellinen)
SELECT t.tili_id, k.kortti_id, 'WITHDRAWAL', 40.00, 'EUR', t.saldo_eur - 40.00
FROM Tili t
JOIN Asiakas a ON a.asiakas_id = t.omistaja_id
JOIN Kortti k ON k.asiakas_id = a.asiakas_id
WHERE t.tilinumero = 'FI00 6666 3333 0000 01' AND k.kortti_numero = '4000-2468-1357-0001';

-- Pelle: 20€ nosto
INSERT INTO Tilitapahtuma(tili_id, kortti_id, laji, summa_eur, valuutta, saldo_hetkellinen)
SELECT t.tili_id, k.kortti_id, 'WITHDRAWAL', 20.00, 'EUR', t.saldo_eur - 20.00
FROM Tili t
JOIN Asiakas a ON a.asiakas_id = t.omistaja_id
JOIN Kortti k ON k.asiakas_id = a.asiakas_id
WHERE t.tilinumero = 'FI00 8888 4444 0000 01' AND k.kortti_numero = '4000-0000-0000-0001';

-- Hannu: debit 10€ nosto, credit 200€ nosto
INSERT INTO Tilitapahtuma(tili_id, kortti_id, laji, summa_eur, valuutta, saldo_hetkellinen)
SELECT t.tili_id, k.kortti_id, 'WITHDRAWAL', 10.00, 'EUR', t.saldo_eur - 10.00
FROM Tili t
JOIN Asiakas a ON a.asiakas_id = t.omistaja_id
JOIN Kortti k ON k.asiakas_id = a.asiakas_id
WHERE t.tilinumero = 'FI00 9999 5555 0000 01' AND k.kortti_numero = '4000-7777-5555-0001';

INSERT INTO Tilitapahtuma(tili_id, kortti_id, laji, summa_eur, valuutta, saldo_hetkellinen)
SELECT t.tili_id, k.kortti_id, 'WITHDRAWAL', 200.00, 'EUR', t.saldo_eur - 200.00
FROM Tili t
JOIN Asiakas a ON a.asiakas_id = t.omistaja_id
JOIN Kortti k ON k.asiakas_id = a.asiakas_id
WHERE t.tilinumero = 'FI00 9999 5555 0000 02' AND k.kortti_numero = '4000-7777-5555-0001';
