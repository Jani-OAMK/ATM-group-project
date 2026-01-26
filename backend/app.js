var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');

const tiliRouter = require('./src/routes/tiliRoutes');
const transaktioRouter = require('./src/routes/transaktioRoutes');

var app = express();

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/transaktioRouter', transaktioRouter);
app.use('/tiliRouter', tiliRouter);

module.exports = app;
