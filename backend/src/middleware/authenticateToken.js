import jwt from 'jsonwebtoken';

export default function authenticateToken(req, res, next) {
  const authHeader = req.headers.authorization;

  console.log('AUTH HEADER:', authHeader);

  if (!authHeader) {
    return res.status(401).json({ message: 'Token puuttuu' });
  }

  const parts = authHeader.split(' ');
  if (parts.length !== 2 || parts[0] !== 'Bearer') {
    return res.status(401).json({ message: 'Virheellinen Authorization-header' });
  }

  const token = parts[1];

  jwt.verify(token, process.env.MY_TOKEN, (err, decoded) => {
    if (err) {
      console.log('JWT ERROR:', err.message);
      return res.status(403).json({ message: 'Token ei kelpaa' });
    }

    req.user = decoded;
    next();
  });
}

