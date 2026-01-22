
//tänne tilijutut
//debug testi kooodi
import express from 'express';
const router = express.Router();

// Add routes here

export default router;


router.get('/:tili_id/debit', function(request, response){
    Tili.getDebitSaldo(request.params.tili_id, function(err,dbResult){
        if (err){
            response.json(err);
        }
        else if (dbResult.length === 0) {
            response.status(404).json({message: 'Käyttötilisi ei ole aktiivinen tai sitä ei löytynyt'});
        }
        else{
            response.json(dbResult[0]);
        }
    });
});

router.get('/:tili_id/credit', function(request, response) {
  Tili.getCreditSaldo(request.params.tili_id, function(err, dbResult) {
    if (err){
      res.json(err);
    } 
    else if(dbResult.length === 0) {
      res.status(404).json({ message: 'Tiliä ei löytynyt' });
    } 
    else{
      res.json(dbResult[0]);
    }
  });
});



module.exports = router;