import {getPool} from '../db.js';

const Transaktio = {

     // Tilitapahtumat-toiminto, näyttää 10 uusinta tapahtumaa
    getTilitapahtumat: async function(tili_id, callback) {
        try {
          const pool = getPool();
          const sql = `
            SELECT tapahtuma_id, laji, summa_eur, tapahtuma_aika
            FROM Tilitapahtuma WHERE tili_id = ? ORDER BY tapahtuma_aika
            DESC LIMIT 10
          `;
          const [rows] = await pool.query(sql, [tili_id]);
          return callback(null, rows);
        } catch (err) {
          return callback(err);
        }
    },

    // käyttösaldon haku
    getKayttosaldo: async function(tili_id, kortti_id, callback) {
        try {
          // Mika tili kyseessa?
          const pool = getPool();

          const [tiliRows] = pool.query("SELECT saldo_eur, credit_limit FROM Tili WHERE tili_id = ?",
                [tili_id]);

          if (tiliRows.length === 0) {
            return callback(new Error("Tiliä ei löytynyt"));
          }

          const { saldo_eur, credit_limit } = tiliRows[0];

          // Hae rooli juuri tälle kortti–tili -yhdistelmalle
          const [rooliRows] = await pool.query(
            'SELECT rooli FROM Korttitili WHERE kortti_id = ? AND tili_id = ?',
            [kortti_id, tili_id]);

          if (rooliRows.length === 0) {
                return callback(new Error("Korttia ei ole liitetty tähän tiliin"));
          }

          const rooli = rooliRows[0].rooli;

          let kayttosaldo = 0;

          if (rooli === "DEBIT") {
                kayttosaldo = saldo_eur;
          } else {  // CREDIT
            kayttosaldo = Number(saldo_eur) + Number(credit_limit);
            if (kayttosaldo < 0) kayttosaldo = 0;
          }

          return callback(null, {rooli, saldo_eur, credit_limit, kayttosaldo});
        } catch (err) {
          return callback(err);
        }
    },

    //Apufunktio: Lisää tilitapahtuma
    addTilitapahtuma: async function(connection, tili_id, kortti_id, laji, summa_eur) {
      const sql = "INSERT INTO Tilitapahtuma (tili_id, kortti_id, laji, summa_eur) VALUES (?, ?, ?, ?) "; 
      const [result] = await connection.query(sql, [tili_id, kortti_id, laji, summa_eur ]); return result;
    },
    postNosta: async function(tili_id, kortti_id, summa_eur, callback) {
        if (summa_eur <= 0) {
          return callback(new Error("Summan täytyy olla positiivinen luku"));
        }

        let connection;

        try {
          const pool = getPool();
          connection = await pool.getConnection();

          await connection.beginTransaction();

          const saldoTiedot = await new Promise((resolve, reject) => {
            this.getKayttosaldo(tili_id, kortti_id, (err, data) => {
              if (err) reject(err);
              else resolve(data);
            });
          });

          const { rooli, saldo_eur, credit_limit } = saldoTiedot;
          const saldo = Number(saldo_eur);
          const limit = Number(credit_limit);

          //  Nostorajoitukset
          if (rooli === "CREDIT") {
            if (saldo - summa_eur < -credit_limit) {
              return callback("Luottoraja ylittyy");
            }
          } else { // DEBIT
            if (summa_eur > saldo) {
              return callback("Ei riittävästi saldoa");
            }
          }

          // Päivitetään saldo
          const [updateResult] = await connection.query(
            'UPDATE Tili SET saldo_eur = saldo_eur - ? WHERE tili_id = ?',
            [summa_eur, tili_id]
          );

          if (updateResult.affectedRows !== 1) {
            return callback("Saldon päivitys tietokantaan epäonnistui");
          }

          await this.addTilitapahtuma(connection, tili_id, kortti_id, "WITHDRAWAL", summa_eur);

          await connection.commit();

          const newSaldo = saldo - summa_eur;
          return  callback(null, { success: true, newSaldo });

        } catch (err) {
          if (connection) await connection.rollback();
          callback(err);
        } finally {
          if (connection) connection.release();
        }
    },

    //Debit-puolella tämä ohjataan Talleta rahaa-vaihtoehtoon, credit-puolella Lyhennä luottoa!
    postTalleta: async function(tili_id, kortti_id, summa_eur, callback) {
      if (summa_eur <= 0) {
        return callback(new Error("Summan täytyy olla positiivinen luku"));
      }

      let connection;

      try {
        const pool = getPool();
        connection = await pool.getConnection();

        await connection.beginTransaction();

        const saldoTiedot = await new Promise((resolve, reject) => {
          this.getKayttosaldo(tili_id, kortti_id, (err, data) => {
            if (err) reject(err);
            else resolve(data);
          });
        });

        const { rooli, saldo_eur, credit_limit, kayttosaldo } = saldoTiedot;
        const saldo = Number(saldo_eur);                               // Muunnetaan string numeroksi jotta callback ynnää ne oikein.

        // Päivitetään saldo
        const [updateResult] = await connection.query(
          'UPDATE Tili SET saldo_eur = saldo_eur + ? WHERE tili_id = ?',
          [summa_eur, tili_id]);

        if (updateResult.affectedRows !== 1) {
          return callback("Saldon päivitys tietokantaan epäonnistui");
        }

        await this.addTilitapahtuma(connection, tili_id, kortti_id, "DEPOSIT", summa_eur);
        await connection.commit();

        const newSaldo = summa_eur + saldo;

        return callback(null, {success: true, newSaldo});

      } catch (err) {
        if (connection) await connection.rollback();
        return callback(err);
      } finally {
        if (connection) connection.release();
      }
   },
};

export default Transaktio;
