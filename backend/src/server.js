
//backend/src/server.js
import express from 'express';
import helmet from 'helmet';
import morgan from 'morgan';
import dotenv from 'dotenv';

import asiakasRoutes from './routes/asiakasRoutes.js';
import authRoutes from './routes/authRoutes.js';
import korttiRoutes from './routes/korttiRoutes.js';
import authenticateToken from './middleware/authenticateToken.js';

dotenv.config();

const app = express();

app.use(helmet());
app.use(express.json());
app.use(morgan('combined'));


app.use('/auth', authRoutes);
app.get('/health', (req, res) => res.json({ ok: true }));


app.use(authenticateToken);
app.use('/kortti', korttiRoutes);
app.use('/asiakas', asiakasRoutes);

const port = process.env.PORT || 3000;
const host = '0.0.0.0';
app.listen(port, host, () => console.log(`API listening on ${host}:${port}`));
