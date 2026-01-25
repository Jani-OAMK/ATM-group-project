
import { getPool } from '../db.js';
import bcrypt from 'bcryptjs';

const kortti = {
  // Hae kaikki asiakkaan kortit
  getByCustomer: async function(customerId, callback) {
    try {
      const pool = await getPool();
      const [rows] = await pool.execute('SELECT * FROM Kortti WHERE asiakas_id = ?', [customerId]);
      callback(null, rows);
    } catch (err) {
      callback(err);
    }
  },

  // Hae tietyn kortin tiedot
  getById: async function(cardId, callback) {
    try {
      const pool = await getPool();
      const [rows] = await pool.execute('SELECT * FROM Kortti WHERE kortti_id = ?', [cardId]);
      callback(null, rows);
    } catch (err) {
      callback(err);
    }
  },

  // Luo uusi kortti
  create: async function(cardData, callback) {
    try {
      if (!cardData.pin || !cardData.asiakas_id || !cardData.kortti_numero) {
        return callback(new Error('puuttuvia tietoja'));
      }
      const pinHash = bcrypt.hashSync(cardData.pin, 10);
      const pool = await getPool();
      const [result] = await pool.execute(
        'INSERT INTO Kortti (kortti_numero, asiakas_id, pin_bcrypt, tila) VALUES (?, ?, ?, ?)',
        [cardData.kortti_numero, cardData.asiakas_id, pinHash, cardData.tila || 'ACTIVE']
      );
      callback(null, { kortti_id: result.insertId, ...cardData });
    } catch (err) {
      callback(err);
    }
  },

  // Hae kortin saldo
  getBalance: async function(cardId, callback) {
    try {
      const pool = await getPool();
      const [rows] = await pool.execute(
        `SELECT t.saldo_eur, t.credit_limit, k.kortti_numero
         FROM Kortti k
         JOIN KorttiTili kt ON k.kortti_id = kt.kortti_id
         JOIN Tili t ON kt.tili_id = t.tili_id
         WHERE k.kortti_id = ? AND kt.rooli = 'DEBIT'`,
        [cardId]
      );
      callback(null, rows[0] || {});
    } catch (err) {
      callback(err);
    }
  },

  // Hae kaikki tapahtumat kortille
  getTransactions: async function(cardId, callback) {
    try {
      const pool = await getPool();
      const [rows] = await pool.execute(
        'SELECT * FROM Tilitapahtuma WHERE kortti_id = ? ORDER BY tapahtuma_aika DESC',
        [cardId]
      );
      callback(null, rows);
    } catch (err) {
      callback(err);
    }
  },

  // Päivitä kortin tila
  updateStatus: async function(cardId, status, callback) {
    try {
      const validStatuses = ['ACTIVE', 'LOCKED', 'CLOSED'];  //tarkasta kaikki määritykset!!
      if (!validStatuses.includes(status)) {
        return callback(new Error);
      }
      const pool = await getPool();
      const [result] = await pool.execute(
        'UPDATE Kortti SET tila = ? WHERE kortti_id = ?',
        [status, cardId]
      );
      callback(null, { affectedRows: result.affectedRows });
    } catch (err) {
      callback(err);
    }
  },

  // Päivitä kortin PIN
  updatePin: async function(cardId, newPin, callback) {
    try {
      if (!newPin || typeof newPin !== 'string' || newPin.length < 4) {
        return callback(new Error);
      }
      const pinHash = bcrypt.hashSync(newPin, 10);
      const pool = await getPool();
      await pool.execute('UPDATE Kortti SET pin_bcrypt = ? WHERE kortti_id = ?', [pinHash, cardId]);
      callback(null, { message: 'Pin koodi vaihdettu' });
    } catch (err) {
      callback(err);
    }
  }
};

export default kortti;