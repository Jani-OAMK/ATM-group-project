import { getPool } from '../db.js';

const asiakas = {
  getAll: async function(callback) {
    try {
      const pool = await getPool();
      const [rows] = await pool.query("SELECT * FROM Asiakas");
      callback(null, rows);
    } catch (err) {
      callback(err);
    }
  },

  getOne: async function(asiakas_id, callback) {
    try {
      const pool = await getPool();
      const [rows] = await pool.query(
        "SELECT * FROM Asiakas WHERE asiakas_id = ?",
        [asiakas_id]
      );
      callback(null, rows);
    } catch (err) {
      callback(err);
    }
  },

  getActive: async function(callback) {
    try {
      const pool = await getPool();
      const [rows] = await pool.query(
        "SELECT * FROM Asiakas WHERE aktiivinen = TRUE"
      );
      callback(null, rows);
    } catch (err) {
      callback(err);
    }
  },

  add: async function(asiakas, callback) {
    try {
      const pool = await getPool();
      const [result] = await pool.query(
        "INSERT INTO Asiakas (etunimi, sukunimi, osoite, puhelinnumero, aktiivinen, luotu_aika, paivitetty_aika) VALUES (?, ?, ?, ?, TRUE, NOW(), NOW())",
        [asiakas.etunimi, asiakas.sukunimi, asiakas.osoite, asiakas.puhelinnumero]
      );
      callback(null, result);
    } catch (err) {
      callback(err);
    }
  },

  update: async function(asiakas, asiakas_id, callback) {
    try {
      const pool = await getPool();
      const [result] = await pool.query(
        "UPDATE Asiakas SET etunimi = ?, sukunimi = ?, osoite = ?, puhelinnumero = ?, paivitetty_aika = NOW() WHERE asiakas_id = ?",
        [asiakas.etunimi, asiakas.sukunimi, asiakas.osoite, asiakas.puhelinnumero, asiakas_id]
      );
      callback(null, result);
    } catch (err) {
      callback(err);
    }
  },

  deactivate: async function(asiakas_id, callback) {
    try {
      const pool = await getPool();
      const [result] = await pool.query(
        "UPDATE Asiakas SET aktiivinen = FALSE WHERE asiakas_id = ?",
        [asiakas_id]
      );
      callback(null, result);
    } catch (err) {
      callback(err);
    }
  },

  getTilit: async function(asiakas_id, callback) {
    try {
      const pool = await getPool();
      const [rows] = await pool.query(
        "SELECT DISTINCT t.* FROM Tili t JOIN Korttitili kt ON kt.tili_id = t.tili_id JOIN Kortti k ON k.kortti_id = kt.kortti_id WHERE k.asiakas_id = ?",
        [asiakas_id]
      );
      callback(null, rows);
    } catch (err) {
      callback(err);
    }
  }
};

export default asiakas;

