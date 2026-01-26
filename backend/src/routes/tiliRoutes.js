//tänne asiakas ja tilit
import express from 'express';
import Tili from '../models/tili_models.js';

const router = express.Router();

//getDebitSaldo
router.get('/:tili_id/debit', function(request, response){
    Tili.getDebitSaldo(request.params.tili_id, function(err,dbResult){
        if (err){
            response.json(err);
        } else if(dbResult.length === 0) {
            response.status(404).json({message: 'Tiliäsi ei löytynyt tai se on suljettu'});
        } else {
            response.json(dbResult[0]);
        }
    });
});

//getCreditSaldo
router.get('/:tili_id/credit', function(request, response) {
  Tili.getCreditSaldo(request.params.tili_id, function(err, dbResult) {
    if (err){
      response.json(err);
    } else if(dbResult.length === 0) {
      response.status(404).json({ message: 'Tiliä ei löytynyt' });
    } else {
      response.json(dbResult[0]);
    }
  });
});

export default router;