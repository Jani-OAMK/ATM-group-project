import { getPool } from '../db.js';
import bcrypt from 'bcryptjs';

const kortti = {

  /**
   * hae kaikki kortit tietylle asiakkaalle
   * @param {number} customerId
   * @param {function} callback 
   * QUERY: SELECT * FROM Kortti WHERE asiakas_id
   */
  getByCustomer: async function(customerId, callback) {
    try {
      // Get Pool connection
      const pool = await getPool();
      const [rows] = await pool.execute('SELECT * FROM Kortti WHERE asiakas_id = ?', [customerId]);
      callback(null, rows);
    } catch (err) {
      callback(err);
    }
  },

  /**
   * Get card details by card ID
   * @param {number} cardId - The card ID (kortti_id)
   * @param {function} callback - Callback function(err, result)
   * QUERY: SELECT * FROM Kortti WHERE kortti_id = ?
   */
  getById: async function(cardId, callback) {
    try {
      // TODO: Get pool connection
      // TODO: Execute query to fetch specific card
      // TODO: Return results via callback
      callback(null, []);
    } catch (err) {
      callback(err);
    }
  },

  /**
   * Create a new card for a customer
   * @param {object} cardData - Object with { customerId, cardType, cardNumber, expiryDate, cvv }
   * @param {function} callback - Callback function(err, result)
   * STEPS:
   * 1. Hash the PIN (need to get PIN from somewhere - check route params)
   * 2. INSERT INTO Kortti (kortti_numero, asiakas_id, pin_bcrypt, tila)
   * 3. Return created card info via callback
   */
  create: async function(cardData, callback) {
    try {
      // TODO: Validate cardData
      // TODO: Generate or hash PIN
      // TODO: Get pool connection
      // TODO: Execute INSERT query
      // TODO: Create PinPool entry for the new card
      // TODO: Return results via callback
      callback(null, {});
    } catch (err) {
      callback(err);
    }
  },

  /**
   * Get card balance (from related Tili account)
   * @param {number} cardId - The card ID (kortti_id)
   * @param {function} callback - Callback function(err, result)
   * QUERY: 
   * SELECT t.saldo_eur, t.credit_limit, k.kortti_numero
   * FROM Kortti k
   * JOIN KorttiTili kt ON k.kortti_id = kt.kortti_id
   * JOIN Tili t ON kt.tili_id = t.tili_id
   * WHERE k.kortti_id = ? AND kt.rooli = 'DEBIT'
   */
  getBalance: async function(cardId, callback) {
    try {
      // TODO: Get pool connection
      // TODO: Execute query to fetch balance from linked account
      // TODO: Return saldo_eur and credit_limit via callback
      callback(null, {});
    } catch (err) {
      callback(err);
    }
  },

  /**
   * Get all transactions for a card
   * @param {number} cardId - The card ID (kortti_id)
   * @param {function} callback - Callback function(err, result)
   * QUERY:
   * SELECT * FROM Tilitapahtuma
   * WHERE kortti_id = ?
   * ORDER BY tapahtuma_aika DESC
   */
  getTransactions: async function(cardId, callback) {
    try {
      // TODO: Get pool connection
      // TODO: Execute query to fetch transactions for card
      // TODO: Order by date descending
      // TODO: Return results via callback
      callback(null, []);
    } catch (err) {
      callback(err);
    }
  },

  /**
   * Update card status (ACTIVE, LOCKED, STOLEN, CLOSED)
   * @param {number} cardId - The card ID (kortti_id)
   * @param {string} status - New status (ACTIVE, LOCKED, STOLEN, CLOSED)
   * @param {function} callback - Callback function(err, result)
   * QUERY: UPDATE Kortti SET tila = ? WHERE kortti_id = ?
   */
  updateStatus: async function(cardId, status, callback) {
    try {
      // TODO: Validate status is one of: ACTIVE, LOCKED, STOLEN, CLOSED
      // TODO: Get pool connection
      // TODO: Execute UPDATE query
      // TODO: Return updated rows info via callback
      callback(null, {});
    } catch (err) {
      callback(err);
    }
  },

  /**
   * Update card PIN
   * @param {number} cardId - The card ID (kortti_id)
   * @param {string} newPin - The new PIN (plain text, will be hashed)
   * @param {function} callback - Callback function(err, result)
   * STEPS:
   * 1. Hash the new PIN using bcryptjs
   * 2. UPDATE Kortti SET pin_bcrypt = ? WHERE kortti_id = ?
   * 3. Reset PIN error counter in PinPool
   */
  updatePin: async function(cardId, newPin, callback) {
    try {
      // TODO: Validate newPin (length, format, etc)
      // TODO: Hash newPin using bcryptjs.hashSync(newPin, 10)
      // TODO: Get pool connection
      // TODO: Execute UPDATE query for pin_bcrypt
      // TODO: Reset virhelaskuri in PinPool table
      // TODO: Return results via callback
      callback(null, {});
    } catch (err) {
      callback(err);
    }
  }

};

export default kortti;
