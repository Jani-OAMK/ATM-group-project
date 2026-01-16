const db = require('../db');



const Transaktio = {

//Tilitapahtumat-toiminto, 10 uusinta tapahtumaa.
getTilitapahtumat: function(tili_id, callback){
    return db.query( 'SELECT tapahtuma_id, laji, summa_eur, tapahtuma_aika FROM tilitapahtuma WHERE tili_id = ? order by tapahtuma_aika DESC LIMIT 10',[tili_id], callback
)};

postNosta: function(tili_id, kortti_id, summa_eur, callback) { 
    // 1) Vähennä saldoa
     db.query( 'UPDATE tili SET saldo_eur = saldo_eur - ? WHERE tili_id = ? AND saldo_eur >= ?', [summa_eur, tili_id, summa_eur],
         function(err, result) { 
            if (err || result.affectedRows === 0) {
                 return callback(err || new Error('Ei riittävästi saldoa')); }
                  // 2) Tehdään nostosta tilitapahtuma
                    db.query( 'INSERT INTO tilitapahtuma (tili_id, kortti_id, laji, summa_eur, tapahtuma_aika) VALUES (?, ?, "nosto", ?, NOW())', [tili_id, kortti_id, summa_eur],
                    function(err, insertResult) {
                        if (err) {
                             return callback(err); 
                        } 
                         callback(null, insertResult); 
                    }); 
    }); 
},


module.exports = Transaktio;