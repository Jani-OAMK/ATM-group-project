import { getPool } from '../db.js';

const Tili = {

    // Debit-tilin saldo
    getDebitSaldo: async function(tili_id, callback) {
        try{
            const pool = await getPool();
            const sql = "SELECT saldo_eur FROM tili WHERE tili_id = ? AND tila = 'ACTIVE' ";
            const [rows] = await pool.query(sql, [tili_id]);
            callback(null, rows);
        } catch (err){
            callback(err);
        }
    },

    // Credit-tilin kaytettavissa-saldo
    getCreditSaldo: async function(tili_id, callback) { 
        try { 
            const pool = await getPool();
            const sql = "SELECT (credit_limit + saldo_eur) FROM tili WHERE tili_id = ?";
            const [rows] = await pool.query(sql, [tili_id]); 
            callback(null, rows); 
        } catch (err){ 
            callback(err); 
        } 
    }
};

export default Tili;
