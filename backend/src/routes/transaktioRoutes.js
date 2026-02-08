import express from 'express';
import Transaktio from '../models/transaktio_models.js';

const router = express.Router();

// getTilitapahtumat (10 rivia)
router.get("/tapahtumat/:tili_id", (req, res) => {
    Transaktio.getTilitapahtumat(req.params.tili_id, (err, result) => {
        if (err) {
            res.send(err); 
        } else {
            res.json(result); 
        } 
    }); 
});

// postKayttosaldo
router.post("/kayttosaldo", (req, res) => {
    const { tili_id, kortti_id } = req.body;

    Transaktio.postKayttosaldo(tili_id, kortti_id, (err, result) => {
        if (err) return res.status(400).json({ error: err.message });
        res.json(result);
    });
});




// postNosta
router.post("/nosta", (req, res) => {
    Transaktio.postNosta(req.body.tili_id,req.body.kortti_id, req.body.summa_eur,(err, result) => {
        if (err) {
            res.send(err);
        } else {
            res.json({message:"Nostotapahtuma onnistui", result });
        }
    });
});

// postTalleta
router.post("/talleta", (req, res) => {
    Transaktio.postTalleta(req.body.tili_id,req.body.kortti_id,req.body.summa_eur, (err, result) => {
        if (err) {
            res.send(err);
        } else {
            res.json({ message: "Talletus onnistui", result });
        }
    });
});

export default router;