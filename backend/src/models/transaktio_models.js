import {getPool} from '../db.js';

const Transaktio = {

     // Tilitapahtumat-toiminto, näyttää 10 uusinta tapahtumaa
    getTilitapahtumat: async function(tili_id, callback) {
        try { 
            const pool = await getPool(); 
            const sql = ` SELECT tapahtuma_id, laji, summa_eur, tapahtuma_aika FROM tilitapahtuma WHERE tili_id = ? ORDER BY tapahtuma_aika DESC LIMIT 10 `; 
            const [rows] = await pool.query(sql, [tili_id]); 
            callback(null, rows); 
        } 
        catch (err) { 
            callback(err); 
        } 
    },


    //Apufunktio pääfunktioihin;tarkistetaan kumppi kortti ja lasketaan kayttosaldo.
    getKayttosaldo: async function(tili_id, callback) {
         try { 
            const pool = await getPool();
            const sql = " SELECT saldo_eur, credit_limit FROM tili WHERE tili_id = ? ";
            const [rows] = await pool.query(sql, [tili_id]);
            
            if (rows.length === 0) {
                 return callback(new Error("Tiliä ei löytynyt")); 
            } 
            const tili = rows[0];

            let tilityyppi;
            if (tili.credit_limit > 0) {
            tilityyppi = "CREDIT";
            } else {
            tilityyppi = "DEBIT";
            }

            let kayttosaldo = 0; 
            if (tilityyppi === "DEBIT") {
                kayttosaldo = tili.saldo_eur;
            } else if(tilityyppi === "CREDIT"){
                kayttosaldo = tili.saldo_eur + tili.credit_limit;
            } else {
                return callback(new Error("Tilityyppiä ei löytynyt"));
            }
            callback(null, { kayttosaldo, tilityyppi, saldo_eur: tili.saldo_eur         
            }); 
        } 
        catch (err) { 
            callback(err);
        }
    },

    //Apufunktio: Lisää tilitapahtuma
    addTilitapahtuma: async function(tili_id, kortti_id, laji, summa_eur, callback) {
        try {
            const pool = await getPool(); 
            const sql = ` INSERT INTO tilitapahtuma (tili_id, kortti_id, laji, summa_eur) VALUES (?, ?, ?, ?) `; 
            const [result] = await pool.query(sql, [ tili_id, kortti_id, laji, summa_eur ]); callback(null, result);
        } 
        catch (err) { 
            callback(err); 
        } 
    },

    postNosta: async function(tili_id, kortti_id, summa_eur, callback) { 
        try { 
            // 1. Haetaan käyttösaldo
            this.getKayttosaldo(tili_id, async (err, data) => { 
                if (err) {
                    return callback(err);
                }
                const { kayttosaldo } = data;
                
                if (kayttosaldo < summa_eur) {
                    return callback(new Error("Ei riittävästi käyttösaldoa"));
                } else {
                    // 2. Päivitä saldo 
                    const pool = await getPool(); 
                    const sql = ` UPDATE tili SET saldo_eur = saldo_eur - ? WHERE tili_id = ? `; 
                    const [result] = await pool.query(sql, [summa_eur, tili_id]); 
                    if (result.affectedRows === 0) { 
                        return callback(new Error("Saldoa ei voitu päivittää")); 
                    } else {
                    // 3. Lisää tilitapahtuma 
                    return this.addTilitapahtuma( tili_id, kortti_id, "WITHDRAWAL", summa_eur, callback ); 
                    }
                }
            }); 
        } 
        catch (err) { 
            callback(err); 
        } 
    },
    // Talletustoiminto; lisätään haluttu saldo, ja tehdään tilitapahtumarivi.
    postTalleta: async function(tili_id, kortti_id, summa_eur, callback) { 
        try { 
            const pool = await getPool(); 
            const sql = ` UPDATE tili SET saldo_eur = saldo_eur + ? WHERE tili_id = ? `; 
            const [result] = await pool.query(sql, [summa_eur, tili_id]); 
            if (result.affectedRows === 0) {
                return callback(new Error("Saldoa ei voitu päivittää"));
            } 
            this.addTilitapahtuma( tili_id, kortti_id, "DEPOSIT", summa_eur, callback ); 
        } 
        catch (err) { 
            callback(err); 
        } 
    }
};


export default Transaktio;