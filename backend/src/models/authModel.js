import { getPool } from '../db.js';
import bcrypt from 'bcryptjs';

const auth = {

  getKorttiByNumero: async function(kortti_numero, callback) {
    try {
      const pool = getPool();
      const [rows] = await pool.query(
        "SELECT * FROM Kortti WHERE kortti_numero = ?",
        [kortti_numero]
      );
      return callback(null, rows);
    } catch (err) {
      return callback(err);
    }
  },

  verifyPin: function(pin, pin_bcrypt, callback) {
    bcrypt.compare(pin, pin_bcrypt, function(err, isMatch) {
      if (err) {
        return callback(err);
      }
      return callback(null, isMatch);
    });
  },

  getPinYrityys: async function(kortti_id, callback) {
    try {
      const pool = getPool();
      const [rows] = await pool.query(
        "SELECT * FROM PinPool WHERE kortti_id = ?",
        [kortti_id]
      );
      return callback(null, rows);
    } catch (err) {
      return callback(err);
    }
  },

  incrementPinError: async function(kortti_id, callback) {
    try {
      const pool = getPool();
      const [result] = await pool.query(
        `UPDATE PinPool 
         SET virhelaskuri = virhelaskuri + 1,
             viimeisin_yritys = NOW()
         WHERE kortti_id = ?`,
        [kortti_id]
      );
      return callback(null, result);
    } catch (err) {
      return callback(err);
    }
  },

  resetPinError: async function(kortti_id, callback) {
    try {
      const pool = getPool();
      const [result] = await pool.query(
        `UPDATE PinPool
         SET virhelaskuri = 0,
             viimeisin_yritys = NOW()
         WHERE kortti_id = ?`,
        [kortti_id]
      );
      return callback(null, result);
    } catch (err) {
      return callback(err);
    }
  },

  lockPinYrityys: async function(kortti_id, callback) {
    try {
      const pool = getPool();
      const [result] = await pool.query(
        "UPDATE PinPool SET lukossa_asti = DATE_ADD(NOW(), INTERVAL 10 MINUTE) WHERE kortti_id = ?",
        [kortti_id]
      );
      return callback(null, result);
    } catch (err) {
      return callback(err);
    }
  },

  lockCard: async function(kortti_id, callback) {
    try {
      const pool = getPool();
      const [result] = await pool.query(
        "UPDATE Kortti SET tila = 'LOCKED' WHERE kortti_id = ?",
        [kortti_id]
      );
      return callback(null, result);
    } catch (err) {
      return callback(err);
    }
  },


  getKorttiTilit: async function (kortti_id, callback) {
    try {
        const pool = getPool();
        const [rows] = await pool.query(`
            SELECT 
                kt.tili_id,
                kt.rooli,
                t.tilinumero,
                t.saldo_eur,
                t.credit_limit,
                t.valuutta,
                t.tila
            FROM KorttiTili kt
            JOIN Tili t ON kt.tili_id = t.tili_id
            WHERE kt.kortti_id = ?
              AND t.tila = 'ACTIVE'
            ORDER BY kt.rooli
        `, [kortti_id]);
        
        return callback(null, rows);
    } catch (err) {
        return callback(err);
    }
  },
};

export default auth;
