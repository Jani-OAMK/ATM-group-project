// backend/src/server.js
import express from 'express';
import helmet from 'helmet';
import morgan from 'morgan';
import dotenv from 'dotenv';

// Routesit
import authRoutes from './routes/authRoutes.js';
import asiakasRoutes from './routes/asiakasRoutes.js';
import korttiRoutes from './routes/korttiRoutes.js';
import tiliRoutes from './routes/tiliRoutes.js';
import transaktioRoutes from './routes/transaktioRoutes.js';

import authenticateToken from './middleware/authenticateToken.js';

dotenv.config();

const app = express();

app.use(helmet());
app.use(express.json());
app.use(morgan('combined'));

app.get('/api/health', (req, res) => res.json({ ok: true }));
app.get('/health', (req, res) => res.json({ ok: true })); //poistetaan testien jälkeen
app.use('/api/auth', authRoutes); 			//public routes, joka ei vaadi tokenia

//app.use(authenticateToken);				//protected route tokenilla

//token guard, kaikki alapuolella on protected, mutta ohittaa OPTIONSIN
app.use((req, res, next) => {
  if (req.method === 'OPTIONS') return next();
  return authenticateToken(req, res, next);
});

app.use('/api/asiakas', asiakasRoutes);
app.use('/api/kortti', korttiRoutes);
app.use('/api/tili', tiliRoutes);
app.use('/api/transaktio', transaktioRoutes);

//JSON 404 ja error handler:
app.use((req, res) => {
  res.status(404).json({
    error: 'Not Found',
    path: req.originalUrl,
  });
});
app.use((err, req, res, next) => {
  console.error(err);
  res.status(err.status || 500).json({
    error: err.message || 'Internal Server Error',
  });
});


app.use(authenticateToken);
app.use('/kortti', korttiRoutes);
app.use('/asiakas', asiakasRoutes);
app.use('/tili', tiliRoutes);
app.use('/transaktio', transaktioRoutes);


const port = process.env.PORT || 3000;
const host = process.env.HOST || '0.0.0.0';
app.listen(port, host, () => console.log(`API listening on ${host}:${port}`));
