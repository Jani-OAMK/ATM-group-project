//tänne kortti juttut
import express from 'express';
import kortti from '../models/korttiModel.js';

const router = express.Router();

// Hae kaikki asiakkaan kortit
router.get('/customer/:customerId', (req, res) => {
  try {
    const { customerId } = req.params;
    
    if (!customerId) {
      return res.status(400).json({ error: 'customerId vaaditaan' });
    }
    
    kortti.getByCustomer(customerId, function(err, result) {
      if (err) return res.status(500).json({ error: err.message });
      res.status(200).json(result);
    });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Hae tietyn kortin tiedot
router.get('/details/:cardId', (req, res) => {
  try {
    const { cardId } = req.params;
    
    if (!cardId) {
      return res.status(400).json({ error: 'cardId vaaditaan' });
    }
    
    kortti.getById(cardId, function(err, result) {
      if (err) return res.status(500).json({ error: err.message });
      if (!result || result.length === 0) return res.status(404).json({ error: 'Korttia ei löydy' });
      res.status(200).json(result[0]);
    });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Luo uusi kortti
router.post('/', (req, res) => {
  try {
    const { customerId, cardType, cardNumber, expiryDate, cvv } = req.body;
    
    if (!customerId || !cardType || !cardNumber) {
      return res.status(400).json({ error: 'customerId, cardType ja cardNumber vaaditaan' });
    }
    
    kortti.create({ customerId, cardType, cardNumber, expiryDate, cvv }, function(err, result) {
      if (err) return res.status(500).json({ error: err.message });
      res.status(201).json(result);
    });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Hae kortin saldo
router.get('/:cardId/balance', (req, res) => {
  try {
    const { cardId } = req.params;
    
    if (!cardId) {
      return res.status(400).json({ error: 'cardId vaaditaan' });
    }
    
    kortti.getBalance(cardId, function(err, result) {
      if (err) return res.status(500).json({ error: err.message });
      res.status(200).json(result);
    });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Hakee kortin tapahtumat
router.get('/:cardId/transactions', (req, res) => {
  try {
    const { cardId } = req.params;
    
    if (!cardId) {
      return res.status(400).json({ error: 'cardId vaaditaan' });
    }
    
    kortti.getTransactions(cardId, function(err, result) {
      if (err) return res.status(500).json({ error: err.message });
      res.status(200).json(result);
    });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Kortin lukitus/avaus
router.put('/:cardId/status', (req, res) => {
  try {
    const { cardId } = req.params;
    const { status } = req.body;
    
    if (!cardId || !status) {
      return res.status(400).json({ error: 'cardId ja status vaaditaan' });
    }
    
    kortti.updateStatus(cardId, status, function(err, result) {
      if (err) return res.status(500).json({ error: err.message });
      res.status(200).json(result);
    });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// PIN:in vaihto
router.put('/:cardId/security', (req, res) => {
  try {
    const { cardId } = req.params;
    const { pin } = req.body;
    
    if (!cardId || !pin) {
      return res.status(400).json({ error: 'cardId ja pin vaaditaan' });
    }
    
    kortti.updatePin(cardId, pin, function(err, result) {
      if (err) return res.status(500).json({ error: err.message });
      res.status(200).json({ message: 'PIN vaihto onnistui' });
    });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

export default router;

