
// backend/src/server.js
import express from 'express';
import helmet from 'helmet';
import morgan from 'morgan';
import dotenv from 'dotenv';

dotenv.config();
const app = express();

app.use(helmet());
app.use(express.json());
app.use(morgan('combined'));

app.get('/health', (req, res) => res.json({ ok: true }));

const port = process.env.PORT || 3000;
const host = '0.0.0.0';
app.listen(port, host, () => console.log(`API listening on ${host}:${port}`));
