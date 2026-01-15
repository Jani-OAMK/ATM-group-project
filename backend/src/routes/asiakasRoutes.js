//tänne asiakas ja tilit
import express from 'express';
import asiakas from '../models/asiakasModel.js';

const router = express.Router();

//kaikki aktiiviset asiakkaat
router.get('/', function(request, response) {
  asiakas.getActive(function(err, result) {
    if (err) {
      response.send(err);
    } else {
      response.json(result);
    }
  })
});

//yksi asiakas
router.get('/:id', function(request, response) {
  asiakas.getOne(request.params.id, function(err, result) {
    if (err) {
      response.send(err);
    } else {
      response.json(result[0]);
    }
  })
});

//lisää asiakas
router.post('/', function(request, response) {
  asiakas.add(request.body, function(err, result) {
    if (err) {
      response.send(err);
    } else {
      response.json(result);
    }
  })
});

//päivitä asiakas
router.put('/:id', function(request, response) {
  asiakas.update(request.body, request.params.id, function(err, result) {
    if (err) {
      response.send(err);
    } else {
      console.log(result.affectedRows);
      response.json(result);
    }
  })
});

//deaktivoi asiakas, ei poistoa koska vaikuttaa tileihin???
router.delete('/:id', function(request, response) {
  asiakas.deactivate(request.params.id, function(err, result) {
    if (err) {
      response.send(err);
    } else {
      console.log(result);
      response.json(result);
    }
  })
});

//hae asiakkaan tilit
router.get('/:id/tilit', function(request, response) {
  asiakas.getTilit(request.params.id, function(err, result) {
    if (err) {
      response.send(err);
    } else {
      response.json(result);
    }
  })
});

export default router;