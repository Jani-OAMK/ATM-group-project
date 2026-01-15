import jwt from 'jsonwebtoken';

const authenticateToken = (req, res, next) => {
  const authHeader = req.headers['authorization'];
  const token = authHeader && authHeader.split(' ')[1]; // Bearer <token>

  if (!token) {
    return res.status(401).json({ error: 'Token puuttuu' });
  }

  const secret = process.env.MY_TOKEN || 'change-this-secret';
  jwt.verify(token, secret, (err, payload) => {
    if (err) {
      return res.status(403).json({ error: 'Virheellinen tai vanhentunut token' });
    }
    req.user = payload;
    next();
  });
};

export default authenticateToken;
