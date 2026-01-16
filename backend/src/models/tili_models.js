const db = require('../db');



const Tili = {

  // Debitin käyttösaldo
  getDebitSaldo: function(tili_id, callback) {
    return query ('SELECT saldo_eur FROM tili WHERE tili_id = ? AND tila = 'ACTIVE' ',
    [tili_id], callback);
  },

  // Creditin käyttösaldo
  getCreditSaldo: function(tili_id, callback) {
    return query ('SELECT (credit_limit + saldo_eur) FROM tili WHERE tili_id = ?', [tili_id], callback);
  },

  //Nostorajan tarkistusta ei voi suorittaa, tietokannasta puuttuu nostorajan asetus-moduuli

  // Mikäli ylläolevan mukainen muutos tehdään, voisi tähän kohtaan tehdä paivittainenraja.nostettu_eur- updaten

  //Saldon paivitys, todennäköisesti turha; transaktiotapahtumassa helpompi päivittää samalla funktiolla kaikki.
  /*updateSaldo: function(tili_id, callback) {
    const query = 'UPDATE tili SET saldo_eur = saldo_eur - ? WHERE tili_id = ?';
    db.query(query, [siirrettavaSumma, tili_id], callback);
  }*/
}

module.exports = Tili;