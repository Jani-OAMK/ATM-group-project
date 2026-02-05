import { getPool } from '../db.js';
import bcrypt from 'bcryptjs';

const auth = {

  getKorttiByNumero: async function(kortti_numero, callback) {
    try {
      const pool = await getPool();
      const [rows] = await pool.query(
        "SELECT * FROM Kortti WHERE kortti_numero = ?",
        [kortti_numero]
      );
      callback(null, rows);
    } catch (err) {
      callback(err);
    }
  },

  verifyPin: function(pin, pin_bcrypt, callback) {
    bcrypt.compare(pin, pin_bcrypt, function(err, isMatch) {
      if (err) {
        return callback(err);
      }
      callback(null, isMatch);
    });
  },

  getPinYrityys: async function(kortti_id, callback) {
    try {
      const pool = await getPool();
      const [rows] = await pool.query(
        "SELECT * FROM PinPool WHERE kortti_id = ?",
        [kortti_id]
      );
      callback(null, rows);
    } catch (err) {
      callback(err);
    }
  },

  incrementPinError: async function(kortti_id, callback) {
    try {
      const pool = await getPool();
      const [result] = await pool.query(
        `UPDATE PinPool 
         SET virhelaskuri = virhelaskuri + 1,
             viimeisin_yritys = NOW()
         WHERE kortti_id = ?`,
        [kortti_id]
      );
      callback(null, result);
    } catch (err) {
      callback(err);
    }
  },

  resetPinError: async function(kortti_id, callback) {
    try {
      const pool = await getPool();
      const [result] = await pool.query(
        `UPDATE PinPool
         SET virhelaskuri = 0,
             viimeisin_yritys = NOW()
         WHERE kortti_id = ?`,
        [kortti_id]
      );
      callback(null, result);
    } catch (err) {
      callback(err);
    }
  },

  lockPinYrityys: async function(kortti_id, callback) {
    try {
      const pool = await getPool();
      const [result] = await pool.query(
        "UPDATE PinPool SET lukossa_asti = DATE_ADD(NOW(), INTERVAL 10 MINUTE) WHERE kortti_id = ?",
        [kortti_id]
      );
      callback(null, result);
    } catch (err) {
      callback(err);
    }
  },

  lockCard: async function(kortti_id, callback) {
    try {
      const pool = await getPool();
      const [result] = await pool.query(
        "UPDATE Kortti SET tila = 'LOCKED' WHERE kortti_id = ?",
        [kortti_id]
      );
      callback(null, result);
    } catch (err) {
      callback(err);
    }
  },  
  
  getKorttiRoolit: async function (kortti_id, callback) {
    try {
        const pool = await getPool();
        const [rows] = await pool.query(
            'SELECT rooli FROM KorttiTili WHERE kortti_id = ?',
            [kortti_id]
        );
        callback(null, rows);
    } catch (err) {
        callback(err);
    }
}

};

export default auth;

