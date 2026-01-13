import express from 'express';
import helmet from 'helmet';
import morgan from 'morgan';
import dotenv from 'dotenv';

import asiakasRoutes from './routes/asiakasRoutes.js';
import authRoutes from './routes/authRoutes.js';
import transaktioRoutes from './routes/transaktioRoutes.js';

dotenv.config();
const app = express();

app.use(helmet());
app.use(express.json());
app.use(morgan('combined'));

app.use('/', asiakasRoutes);
app.use('/', authRoutes);
app.use('/', transaktioRoutes);

app.get('/health', (req, res) => res.json({ ok: true }));

const port = process.env.PORT || 3000;
app.listen(port, () => console.log(`API listening on ${port}`));
