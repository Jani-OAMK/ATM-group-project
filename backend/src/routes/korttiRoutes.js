//tänne kortti juttut
import express from 'express';
const router = express.Router();

// Hae kaikki asiakkaan kortit
router.get('/customer/:customerId', async (req, res) => {
  try {
    const { customerId } = req.params;
    res.status(200).json({ message: 'hae kaikki kortit' });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Hae tietyn kortin tiedot
router.get('/:cardId', async (req, res) => {
  try {
    const { cardId } = req.params;
    res.status(200).json({ message: 'kortin tiedot' });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Luo uusi kortti
router.post('/', async (req, res) => {
  try {
    const { customerId, cardType, cardNumber, expiryDate, cvv } = req.body;
    res.status(201).json({ message: 'korttiluotu' });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Hae kortin saldo tai luottoraja
router.get('/:cardId/balance', async (req, res) => {
  try {
    const { cardId } = req.params;
    res.status(200).json({ message: 'hae kortin balance' });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// Hakee kortin tapahtumat
router.get('/:cardId/transactions', async (req, res) => {
  try {
    const { cardId } = req.params;
    res.status(200).json({ message: 'hae kortin tilitapahtumat' });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// kortin lukitus 
router.put('/:cardId/status', async (req, res) => {
  try {
    const { cardId } = req.params;
    const { status } = req.body; 
    res.status(200).json({ message: 'korttin lukitus muutttunut' });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

// pinin vaihto
router.put('/:cardId/security', async (req, res) => {
  try {
    const { cardId } = req.params;
    const { pin } = req.body;
    res.status(200).json({ message: 'pinkoodin vaihto onnistui' });
  } catch (error) {
    res.status(500).json({ error: error.message });
  }
});

export default router;

